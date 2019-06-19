#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "game_cl_base.h"
#include "game_sv_base.h"
#include "xrmessages.h"
#include "xrEngine/x_ray.h"
#include "xrEngine/device.h"
#include "xrEngine/IGame_Persistent.h"
#include "xrEngine/xr_ioconsole.h"
#include "MainMenu.h"
#include "string_table.h"
#include "UIGameCustom.h"

int g_cl_save_demo = 0;

extern XRCORE_API bool g_allow_heap_min;

bool CLevel::net_Start(const char* op_server, const char* op_client)
{
    net_start_result_total = TRUE;

    pApp->LoadBegin();

	string64 player_name = "";
    xr_strcpy(player_name, xr_strlen(Core.UserName) ? Core.UserName : Core.CompName);
	VERIFY( xr_strlen(player_name) );

    string512 tmp;
    xr_strcpy(tmp, op_client);
    xr_strcat(tmp, "/name=");
    xr_strcat(tmp, player_name);
    m_caClientOptions = tmp;
    m_caServerOptions = op_server;
    //---------------------------------------------------------------------
    g_loading_events.push_back(LOADING_EVENT(this, &CLevel::net_start1));
    g_loading_events.push_back(LOADING_EVENT(this, &CLevel::net_start2));
    g_loading_events.push_back(LOADING_EVENT(this, &CLevel::net_start3));
    g_loading_events.push_back(LOADING_EVENT(this, &CLevel::net_start4));
    g_loading_events.push_back(LOADING_EVENT(this, &CLevel::net_start5));
    g_loading_events.push_back(LOADING_EVENT(this, &CLevel::net_start6));

    return net_start_result_total;
}

bool CLevel::net_start1()
{
    // Start client and server if need it
    if (m_caServerOptions.size())
    {
        g_pGamePersistent->SetLoadStageTitle("st_server_starting");
        g_pGamePersistent->LoadTitle();

		typedef IGame_Persistent::params params;
		params &p = g_pGamePersistent->m_game_params;
		// Connect
		Server = new xrServer();

        if (xr_strcmp(p.m_alife, "alife"))
        {
            shared_str l_ver = game_sv_GameState::parse_level_version(m_caServerOptions);

            map_data.m_name = game_sv_GameState::parse_level_name(m_caServerOptions);

            if (!GEnv.isDedicatedServer)
                g_pGamePersistent->LoadTitle(true, map_data.m_name);

            int id = pApp->Level_ID(map_data.m_name.c_str(), l_ver.c_str(), true);

            if (id < 0)
            {
                Log("Can't find level: ", map_data.m_name.c_str());
                net_start_result_total = FALSE;
                return true;
            }
        }
    }
    else
    {
        g_allow_heap_min = false;
    }

    return true;
}

bool CLevel::net_start2()
{
    if (net_start_result_total && m_caServerOptions.size())
    {
        GameDescriptionData game_descr;
        if ((m_connect_server_err = Server->Connect(m_caServerOptions, game_descr)) != xrServer::ErrNoError)
        {
            net_start_result_total = false;
            Msg("! Failed to start server.");
            return true;
        }
        Server->SLS_Default();
        map_data.m_name = Server->level_name(m_caServerOptions);
        if (!GEnv.isDedicatedServer)
            g_pGamePersistent->LoadTitle(true, map_data.m_name);
    }
    return true;
}

bool CLevel::net_start3()
{
    if (!net_start_result_total)
        return true;
    // add server port if don't have one in options
    if (!strstr(m_caClientOptions.c_str(), "port=") && Server)
    {
        string64 PortStr;
        xr_sprintf(PortStr, "/port=%d", Server->GetPort());

        string4096 tmp;
        xr_strcpy(tmp, m_caClientOptions.c_str());
        xr_strcat(tmp, PortStr);

        m_caClientOptions = tmp;
    }
    // add password string to client, if don't have one
    if (m_caServerOptions.size())
    {
        if (strstr(m_caServerOptions.c_str(), "psw=") && !strstr(m_caClientOptions.c_str(), "psw="))
        {
            string64 PasswordStr = "";
            const char* PSW = strstr(m_caServerOptions.c_str(), "psw=") + 4;
            if (strchr(PSW, '/'))
                strncpy_s(PasswordStr, PSW, strchr(PSW, '/') - PSW);
            else
                xr_strcpy(PasswordStr, PSW);

			string4096	tmp;
			xr_sprintf(tmp, "%s/psw=%s", m_caClientOptions.c_str(), PasswordStr);
			m_caClientOptions = tmp;
		};
	};
	return true;
}

bool CLevel::net_start4()
{
    if (!net_start_result_total)
        return true;

    g_loading_events.pop_front();

    g_loading_events.push_front(LOADING_EVENT(this, &CLevel::net_start_client6));
    g_loading_events.push_front(LOADING_EVENT(this, &CLevel::net_start_client5));
    g_loading_events.push_front(LOADING_EVENT(this, &CLevel::net_start_client4));
    g_loading_events.push_front(LOADING_EVENT(this, &CLevel::net_start_client3));
    g_loading_events.push_front(LOADING_EVENT(this, &CLevel::net_start_client2));
    g_loading_events.push_front(LOADING_EVENT(this, &CLevel::net_start_client1));

    return false;
}

bool CLevel::net_start5()
{
    if (net_start_result_total)
    {
        NET_Packet NP;
        NP.w_begin(M_CLIENTREADY);
        Game().local_player->net_Export(NP, TRUE);
        Send(NP, net_flags(TRUE, TRUE));

        if (OnClient() && Server)
        {
            Server->SLS_Clear();
        };
    };
    return true;
}
bool CLevel::net_start6()
{
    // init bullet manager
    BulletManager().Clear();
    BulletManager().Load();

    pApp->LoadEnd();

    if (net_start_result_total)
    {
        if (strstr(Core.Params, "-$"))
        {
            string256 buf, cmd, param;
            sscanf(strstr(Core.Params, "-$") + 2, "%[^ ] %[^ ] ", cmd, param);
            strconcat(sizeof(buf), buf, cmd, " ", param);
            Console->Execute(buf);
        }
    }
    else
    {
        Msg("! Failed to start client. Check the connection or level existance.");

        if (m_connect_server_err == xrServer::ErrConnect && !psNET_direct_connect && !GEnv.isDedicatedServer)
        {
            DEL_INSTANCE(g_pGameLevel);
            Console->Execute("main_menu on");

        }
        else if (!map_data.m_map_loaded && map_data.m_name.size() && m_bConnectResult)
        {
            LPCSTR level_id_string = NULL;
            LPCSTR dialog_string = NULL;
            LPCSTR download_url = !!map_data.m_map_download_url ? map_data.m_map_download_url.c_str() : "";
            CStringTable st;
            LPCSTR tmp_map_ver = !!map_data.m_map_version ? map_data.m_map_version.c_str() : "";

            STRCONCAT(level_id_string, st.translate("st_level"), ":", map_data.m_name.c_str(), "(", tmp_map_ver, "). ");
            STRCONCAT(dialog_string, level_id_string, st.translate("ui_st_map_not_found"));

			DEL_INSTANCE	(g_pGameLevel);
			Console->Execute("main_menu on");
		}
		else
		if (map_data.IsInvalidClientChecksum())
		{
			LPCSTR level_id_string = NULL;
			LPCSTR dialog_string = NULL;
			LPCSTR download_url = !!map_data.m_map_download_url ? map_data.m_map_download_url.c_str() : "";
			CStringTable	st;
			LPCSTR tmp_map_ver = !!map_data.m_map_version ? map_data.m_map_version.c_str() : "";

            STRCONCAT(level_id_string, st.translate("st_level"), ":", map_data.m_name.c_str(), "(", tmp_map_ver, "). ");
            STRCONCAT(dialog_string, level_id_string, st.translate("ui_st_map_data_corrupted"));

			g_pGameLevel->net_Stop();
			DEL_INSTANCE	(g_pGameLevel);
			Console->Execute("main_menu on");
		}
		else 
		{
			DEL_INSTANCE	(g_pGameLevel);
			Console->Execute("main_menu on");
		}

        return true;
    }

    if (!GEnv.isDedicatedServer)
    {
        if (CurrentGameUI())
            CurrentGameUI()->OnConnected();
    }

    return true;
}

void CLevel::InitializeClientGame(NET_Packet& P)
{
    string256 game_type_name;
    P.r_stringZ(game_type_name);
    if (game && !xr_strcmp(game_type_name, game->type_name()))
        return;

    xr_delete(game);
#ifdef DEBUG
    Msg("- Game configuring : Started ");
#endif // #ifdef DEBUG
    CLASS_ID clsid = game_GameState::getCLASS_ID(game_type_name, false);
    game = smart_cast<game_cl_GameState*>(NEW_INSTANCE(clsid));
    game->set_type_name(game_type_name);
    game->Init();
    m_bGameConfigStarted = TRUE;

    R_ASSERT(Load_GameSpecific_After());
}