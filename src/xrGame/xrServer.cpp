// xrServer.cpp: implementation of the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "xrServer.h"
#include "xrMessages.h"
#include "xrServer_Objects_ALife_All.h"
#include "Level.h"
#include "game_cl_base.h"
#include "game_sv_base.h"
#include "ai_space.h"
#include "xrEngine/IGame_Persistent.h"
#include "string_table.h"
#include "Common/object_broker.h"
#include "xrEngine/Engine.h"
#include "xrEngine/GameFont.h"
#include "xrEngine/PerformanceAlert.hpp"
#include "xrEngine/XR_IOConsole.h"
#include "ui/UIInventoryUtilities.h"
#include "xrNetServer/NET_Messages.h"
#include <functional>

#pragma warning(push)
#pragma warning(disable : 4995)
#include <malloc.h>
#pragma warning(pop)

xrClientData::xrClientData() : IClient(Device.GetTimerGlobal())
{
    ps = NULL;
    Clear();
}

void xrClientData::Clear()
{
    owner = NULL;
    net_Ready = FALSE;
    net_Accepted = FALSE;
    net_PassUpdates = TRUE;
};

xrClientData::~xrClientData() { xr_delete(ps); }
xrServer::xrServer() : IPureServer(Device.GetTimerGlobal(), GEnv.isDedicatedServer)
{
    m_aDelayedPackets.clear();
    m_last_updates_size = 0;
    m_last_update_time = 0;
}

xrServer::~xrServer()
{
    struct ClientDestroyer
    {
        static bool true_generator(IClient*) { return true; }
    };
    IClient* tmp_client = net_players.GetFoundClient(&ClientDestroyer::true_generator);
    while (tmp_client)
    {
        client_Destroy(tmp_client);
        tmp_client = net_players.GetFoundClient(&ClientDestroyer::true_generator);
    }
    m_aDelayedPackets.clear();
    entities.clear();

}

//--------------------------------------------------------------------

CSE_Abstract* xrServer::ID_to_entity(u16 ID)
{
    // #pragma todo("??? to all : ID_to_entity - must be replaced to 'game->entity_from_eid()'")
    if (0xffff == ID)
        return 0;
    xrS_entities::iterator I = entities.find(ID);
    if (entities.end() != I)
        return I->second;
    else
        return 0;
}

//--------------------------------------------------------------------
IClient* xrServer::client_Create() { return new xrClientData(); }
void xrServer::client_Replicate() {}
IClient* xrServer::client_Find_Get(ClientID ID)
{
    IClient* newCL = client_Create();
    newCL->ID = ID;

    newCL->server = this;
    net_players.AddNewClient(newCL);

#ifndef MASTER_GOLD
    Msg("# New player created.");
#endif // #ifndef MASTER_GOLD
    return newCL;
};

u32 g_sv_Client_Reconnect_Time = 3;

void xrServer::client_Destroy(IClient* C)
{
    // Delete assosiated entity
    IClient* alife_client = net_players.FindAndEraseClient(std::bind(std::equal_to<IClient*>(), C, std::placeholders::_1));
    if (alife_client)
    {
        CSE_Abstract* pOwner = static_cast<xrClientData*>(alife_client)->owner;
        CSE_Spectator* pS = smart_cast<CSE_Spectator*>(pOwner);
        if (pS)
        {
            NET_Packet P;
            P.w_begin(M_EVENT);
            P.w_u32(Level().timeServer()); // Device.TimerAsync());
            P.w_u16(GE_DESTROY);
            P.w_u16(pS->ID);
            SendBroadcast(C->ID, P, net_flags(TRUE, TRUE));
        };

        DelayedPacket pp;
        pp.SenderID = alife_client->ID;
        xr_deque<DelayedPacket>::iterator it;
        do
        {
            it = std::find(m_aDelayedPackets.begin(), m_aDelayedPackets.end(), pp);
            if (it != m_aDelayedPackets.end())
            {
                m_aDelayedPackets.erase(it);
                Msg("removing packet from delayed event storage");
            }
            else
                break;
        } while (true);

        if (pOwner)
        {
            game->CleanDelayedEventFor(pOwner->ID);
        }
    }
}

//--------------------------------------------------------------------
int g_Dump_Update_Write = 0;

#ifdef DEBUG
INT g_sv_SendUpdate = 0;
#endif

void xrServer::Update()
{
#ifdef DEBUG
    VERIFY(verify_entities());
#endif

    ProceedDelayedPackets();
    // game update
    game->ProcessDelayedEvent();
    game->Update();

    if (game->sv_force_sync)
        Perform_game_export();

#ifdef DEBUG
    VERIFY(verify_entities());
#endif
}

xr_vector<shared_str> _tmp_log;
void console_log_cb(void* context, const char* text) { _tmp_log.push_back(text); }
u32 xrServer::OnDelayedMessage(NET_Packet& P, ClientID sender) // Non-Zero means broadcasting with "flags" as returned
{
    u16 type;
    P.r_begin(type);

    // csPlayers.Enter			();

#ifdef DEBUG
    VERIFY(verify_entities());
#endif
    xrClientData* CL = ID_to_client(sender);
    // R_ASSERT2						(CL, make_string("packet type [%d]",type).c_str());

	switch (type)
	{
		case M_CLIENT_REQUEST_CONNECTION_DATA:
		{
			IClient* tmp_client = net_players.GetFoundClient(
				ClientIdSearchPredicate(sender));
			VERIFY(tmp_client);
			OnCL_Connected(tmp_client);
		}break;
		case M_FILE_TRANSFER:
		{

		}break;
	}
#ifdef DEBUG
    VERIFY(verify_entities());
#endif

    // csPlayers.Leave					();
    return 0;
}

u32 xrServer::OnMessageSync(NET_Packet& P, ClientID sender)
{
    csMessage.Enter();
    u32 ret = OnMessage(P, sender);
    csMessage.Leave();
    return ret;
}

extern float g_fCatchObjectTime;
u32 xrServer::OnMessage(NET_Packet& P, ClientID sender) // Non-Zero means broadcasting with "flags" as returned
{
    u16 type;
    P.r_begin(type);

#ifdef DEBUG
    VERIFY(verify_entities());
#endif
    xrClientData* CL = ID_to_client(sender);

    switch (type)
    {
    case M_UPDATE:
    {
        Process_update(P, sender); // No broadcast
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_SPAWN:
    {
        if (CL->flags.bLocal)
            Process_spawn(P, sender);

#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_EVENT:
    {
        Process_event(P, sender);
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_EVENT_PACK:
    {
        NET_Packet tmpP;
        while (!P.r_eof())
        {
            tmpP.B.count = P.r_u8();
            P.r(&tmpP.B.data, tmpP.B.count);

            OnMessage(tmpP, sender);
        };
    }
    break;
    case M_CL_UPDATE:
    {
        xrClientData* CL = ID_to_client(sender);
        if (!CL)
            break;
        CL->net_Ready = TRUE;

        if (!CL->net_PassUpdates)
            break;
        //-------------------------------------------------------------------
        u32 ClientPing = CL->stats.getPing();
        P.w_seek(P.r_tell() + 2, &ClientPing, 4);
        //-------------------------------------------------------------------
        if (SV_Client)
            SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_MOVE_PLAYERS_RESPOND:
    {
        xrClientData* CL = ID_to_client(sender);
        if (!CL)
            break;
        CL->net_Ready = TRUE;
        CL->net_PassUpdates = TRUE;
    }
    break;
    //-------------------------------------------------------------------
    case M_CL_INPUT:
    {
        xrClientData* CL = ID_to_client(sender);
        if (CL)
            CL->net_Ready = TRUE;
        if (SV_Client)
            SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_GAMEMESSAGE:
    {
        SendBroadcast(BroadcastCID, P, net_flags(TRUE, TRUE));
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_CLIENTREADY:
    {
        game->OnPlayerConnectFinished(sender);
        // game->signal_Syncronize	();
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_SWITCH_DISTANCE:
    {
        game->switch_distance(P, sender);
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_CHANGE_LEVEL:
    {
        if (game->change_level(P, sender))
        {
            SendBroadcast(BroadcastCID, P, net_flags(TRUE, TRUE));
        }
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_SAVE_GAME:
    {
        game->save_game(P, sender);
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_LOAD_GAME:
    {
        game->load_game(P, sender);
        SendBroadcast(BroadcastCID, P, net_flags(TRUE, TRUE));
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_RELOAD_GAME:
    {
        SendBroadcast(BroadcastCID, P, net_flags(TRUE, TRUE));
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
    }
    break;
    case M_SAVE_PACKET:
    {
        Process_save(P, sender);
#ifdef DEBUG
        VERIFY(verify_entities());
#endif
		}break;
	case M_CLIENT_REQUEST_CONNECTION_DATA:
		{
			AddDelayedPacket(P, sender);
		}break;
	case M_SV_MAP_NAME:
		{
			xrClientData *l_pC			= ID_to_client(sender);
			OnProcessClientMapData		(P, l_pC->ID);
		}break;
	case M_CHANGE_LEVEL_GAME:
		{
			ClientID CID; CID.set		(0xffffffff);
			SendBroadcast				(CID,P,net_flags(TRUE,TRUE));
		}break;
	case M_CL_AUTH:
		{
			game->AddDelayedEvent		(P,GAME_EVENT_PLAYER_AUTH, 0, sender);
		}break;
	case M_CREATE_PLAYER_STATE:
		{
			game->AddDelayedEvent		(P,GAME_EVENT_CREATE_PLAYER_STATE, 0, sender);
		}break;
	case M_STATISTIC_UPDATE:
		{
			SendBroadcast			(BroadcastCID,P,net_flags(TRUE,TRUE));
		}break;
	case M_STATISTIC_UPDATE_RESPOND:
		{

		}break;
	case M_PLAYER_FIRE:
		{
		}break;

    case M_BATTLEYE: {
    }
    break;
    case M_FILE_TRANSFER: { AddDelayedPacket(P, sender);
    }
    break;
    }

#ifdef DEBUG
    VERIFY(verify_entities());
#endif

    return IPureServer::OnMessage(P, sender);
}

void xrServer::SendTo_LL(ClientID ID, void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
    Level().OnMessage(data, size);
}
void xrServer::SendBroadcast(ClientID exclude, NET_Packet& P, u32 dwFlags)
{
    struct ClientExcluderPredicate
    {
        ClientID id_to_exclude;
        ClientExcluderPredicate(ClientID exclude) : id_to_exclude(exclude) {}
        bool operator()(IClient* client)
        {
            xrClientData* tmp_client = static_cast<xrClientData*>(client);
            if (client->ID == id_to_exclude)
                return false;
            if (!client->flags.bConnected)
                return false;
            if (!tmp_client->net_Accepted)
                return false;
            return true;
        }
    };
    struct ClientSenderFunctor
    {
        xrServer* m_owner;
        void* m_data;
        u32 m_size;
        u32 m_dwFlags;
        ClientSenderFunctor(xrServer* owner, void* data, u32 size, u32 dwFlags)
            : m_owner(owner), m_data(data), m_size(size), m_dwFlags(dwFlags)
        {
        }
        void operator()(IClient* client) { m_owner->SendTo_LL(client->ID, m_data, m_size, m_dwFlags); }
    };
    ClientSenderFunctor temp_functor(this, P.B.data, P.B.count, dwFlags);
    net_players.ForFoundClientsDo(ClientExcluderPredicate(exclude), temp_functor);
}
//--------------------------------------------------------------------
CSE_Abstract* xrServer::entity_Create(pcstr name) { return F_entity_Create(name); }
void xrServer::entity_Destroy(CSE_Abstract*& P)
{
#ifdef DEBUG
    if (dbg_net_Draw_Flags.test(dbg_destroy))
        Msg("xrServer::entity_Destroy : [%d][%s][%s]", P->ID, P->name(), P->name_replace());
#endif
    R_ASSERT(P);
    entities.erase(P->ID);
    m_tID_Generator.vfFreeID(P->ID, Device.TimerAsync());

    if (P->owner && P->owner->owner == P)
        P->owner->owner = NULL;

    P->owner = NULL;
    if (!ai().get_alife() || !P->m_bALifeControl)
    {
        F_entity_Destroy(P);
    }
}

//--------------------------------------------------------------------
void xrServer::Server_Client_Check(IClient* CL)
{
    if (SV_Client && SV_Client->ID == CL->ID)
    {
        if (!CL->flags.bConnected)
        {
            SV_Client = NULL;
        };
        return;
    };

    if (SV_Client && SV_Client->ID != CL->ID)
    {
        return;
    };

    if (!CL->flags.bConnected)
    {
        return;
    };

    if (CL->process_id == GetCurrentProcessId())
    {
        CL->flags.bLocal = 1;
        SV_Client = (xrClientData*)CL;
        Msg("New SV client 0x%08x", SV_Client->ID.value());
    }
    else
    {
        CL->flags.bLocal = 0;
    }
};

CSE_Abstract* xrServer::GetEntity(u32 Num)
{
    xrS_entities::iterator I = entities.begin(), E = entities.end();
    for (u32 C = 0; I != E; ++I, ++C)
    {
        if (C == Num)
            return I->second;
    };
    return NULL;
};

#ifdef DEBUG

static BOOL _ve_initialized = FALSE;
static BOOL _ve_use = TRUE;

bool xrServer::verify_entities() const
{
    if (!_ve_initialized)
    {
        _ve_initialized = TRUE;
        if (strstr(Core.Params, "-~ve"))
            _ve_use = FALSE;
    }
    if (!_ve_use)
        return true;

    xrS_entities::const_iterator I = entities.begin();
    xrS_entities::const_iterator E = entities.end();
    for (; I != E; ++I)
    {
        VERIFY2((*I).first != 0xffff, "SERVER : Invalid entity id as a map key - 0xffff");
        VERIFY2((*I).second, "SERVER : Null entity object in the map");
        VERIFY3((*I).first == (*I).second->ID,
            "SERVER : ID mismatch - map key doesn't correspond to the real entity ID", (*I).second->name_replace());
        verify_entity((*I).second);
    }
    return (true);
}

void xrServer::verify_entity(const CSE_Abstract* entity) const
{
    VERIFY(entity->m_wVersion != 0);
    if (entity->ID_Parent != 0xffff)
    {
        xrS_entities::const_iterator J = entities.find(entity->ID_Parent);
        VERIFY2(J != entities.end(),
            make_string("SERVER : Cannot find parent in the map [%s][%s]", entity->name_replace(), entity->name())
                .c_str());
        VERIFY3((*J).second, "SERVER : Null entity object in the map", entity->name_replace());
        VERIFY3((*J).first == (*J).second->ID,
            "SERVER : ID mismatch - map key doesn't correspond to the real entity ID", (*J).second->name_replace());
        VERIFY3(std::find((*J).second->children.begin(), (*J).second->children.end(), entity->ID) !=
                (*J).second->children.end(),
            "SERVER : Parent/Children relationship mismatch - Object has parent, but corresponding parent doesn't have "
            "children",
            (*J).second->name_replace());
    }

    xr_vector<u16>::const_iterator I = entity->children.begin();
    xr_vector<u16>::const_iterator E = entity->children.end();
    for (; I != E; ++I)
    {
        VERIFY3(*I != 0xffff, "SERVER : Invalid entity children id - 0xffff", entity->name_replace());
        xrS_entities::const_iterator J = entities.find(*I);
        VERIFY3(J != entities.end(), "SERVER : Cannot find children in the map", entity->name_replace());
        VERIFY3((*J).second, "SERVER : Null entity object in the map", entity->name_replace());
        VERIFY3((*J).first == (*J).second->ID,
            "SERVER : ID mismatch - map key doesn't correspond to the real entity ID", (*J).second->name_replace());
        VERIFY3((*J).second->ID_Parent == entity->ID,
            "SERVER : Parent/Children relationship mismatch - Object has children, but children doesn't have parent",
            (*J).second->name_replace());
    }
}

#endif // DEBUG

shared_str xrServer::level_name(const shared_str& server_options) const { return (game->level_name(server_options)); }
shared_str xrServer::level_version(const shared_str& server_options) const
{
    return (game_sv_GameState::parse_level_version(server_options));
}

void xrServer::create_direct_client()
{
    SClientConnectData cl_data;
    cl_data.clientID.set(1);
    xr_strcpy(cl_data.name, "single_player");
    cl_data.process_id = GetCurrentProcessId();

    new_client(&cl_data);
}

void xrServer::ProceedDelayedPackets()
{
    DelayedPackestCS.Enter();
    while (!m_aDelayedPackets.empty())
    {
        DelayedPacket& DPacket = *m_aDelayedPackets.begin();
        OnDelayedMessage(DPacket.Packet, DPacket.SenderID);
        //		OnMessage(DPacket.Packet, DPacket.SenderID);
        m_aDelayedPackets.pop_front();
    }
    DelayedPackestCS.Leave();
};

void xrServer::AddDelayedPacket(NET_Packet& Packet, ClientID Sender)
{
    DelayedPackestCS.Enter();

    m_aDelayedPackets.push_back(DelayedPacket());
    DelayedPacket* NewPacket = &(m_aDelayedPackets.back());
    NewPacket->SenderID = Sender;
    CopyMemory(&(NewPacket->Packet), &Packet, sizeof(NET_Packet));

    DelayedPackestCS.Leave();
}

//xr_token game_types[];
void xrServer::GetServerInfo( CServerInfo* si )
{
    string32 tmp;
    string256 tmp256;

    si->AddItem("Server port", xr_itoa(GetPort(), tmp, 10), RGB(128, 128, 255));
    LPCSTR time =
        InventoryUtilities::GetTimeAsString(Device.dwTimeGlobal, InventoryUtilities::etpTimeToSecondsAndDay).c_str();
    si->AddItem("Uptime", time, RGB(255, 228, 0));

//	xr_strcpy( tmp256, get_token_name(game_types, game->Type() ) );
	xr_strcpy( tmp256, "single" );
	
	//if ( g_sv_dm_dwTimeLimit > 0 )
	{
		xr_strcat( tmp256, " time limit [" );
		xr_strcat( tmp256, itoa( 0, tmp, 10 ) );
		xr_strcat( tmp256, "] " );
	}

	si->AddItem( "Game type", tmp256, RGB(128,255,255) );

	if ( g_pGameLevel )
	{
		time = InventoryUtilities::GetGameTimeAsString( InventoryUtilities::etpTimeToMinutes ).c_str();
		
		xr_strcpy( tmp256, time );

		si->AddItem( "Game time", tmp256, RGB(205,228,178) );
	}
}
