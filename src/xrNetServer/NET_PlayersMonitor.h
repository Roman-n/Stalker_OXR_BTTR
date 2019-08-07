#pragma once
#ifndef NET_PLAYERS_MONITOR
#define NET_PLAYERS_MONITOR

#include "NET_Shared.h"
#include "xrCore/fastdelegate.h"
#include "xrCore/Threading/Lock.hpp"

class IClient;

class PlayersMonitor
{
    using players_collection_t = xr_vector<IClient*>;
    Lock csPlayers;
    players_collection_t net_Players;
    bool now_iterating_in_net_players;
public:
    PlayersMonitor()
    {
        now_iterating_in_net_players = false;

    }

    template <typename ActionFunctor>
    void ForEachClientDo(ActionFunctor& functor)
    {
        // Msg("-S- Entering to csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Enter();
        // LogStackTrace(
        //	make_string("-S- Entered to csPlayers [%d]", GetCurrentThreadId()).c_str());
        now_iterating_in_net_players = true;

        for (players_collection_t::iterator i = net_Players.begin(), ie = net_Players.end(); i != ie; ++i)
        {
            VERIFY2(*i != NULL, "IClient ptr is NULL");
            functor(*i);
        }
        now_iterating_in_net_players = false;
        // Msg("-S- Leaving from csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Leave();
    }

    void ForEachClientDo(fastdelegate::FastDelegate1<IClient*, void>& fast_delegate)
    {
        // Msg("-S- Entering to csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Enter();
        // LogStackTrace(
        //	make_string("-S- Entered to csPlayers [%d]", GetCurrentThreadId()).c_str());
        now_iterating_in_net_players = true;

        for (players_collection_t::iterator i = net_Players.begin(), ie = net_Players.end(); i != ie; ++i)
        {
            VERIFY2(*i != NULL, "IClient ptr is NULL");
            fast_delegate(*i);
        }
        now_iterating_in_net_players = false;
        // Msg("-S- Leaving from csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Leave();
    }

    template <typename SearchPredicate, typename ActionFunctor>
    u32 ForFoundClientsDo(SearchPredicate const& predicate, ActionFunctor& functor)
    {
        u32 ret_count = 0;
        // Msg("-S- Entering to csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Enter();
        // LogStackTrace(
        //	make_string("-S- Entered to csPlayers [%d]", GetCurrentThreadId()).c_str());
        now_iterating_in_net_players = true;

        players_collection_t::iterator players_endi = net_Players.end();
        players_collection_t::iterator temp_iter = std::find_if(net_Players.begin(), players_endi, predicate);

        while (temp_iter != players_endi)
        {
            VERIFY2(*temp_iter != NULL, "IClient ptr is NULL");
            functor(*temp_iter);
            temp_iter = std::find_if(++temp_iter, players_endi, predicate);
        }
        now_iterating_in_net_players = false;
        // Msg("-S- Leaving from csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Leave();
        return ret_count;
    }

    template <typename SearchPredicate>
    IClient* FindAndEraseClient(SearchPredicate const& predicate)
    {
        // Msg("-S- Entering to csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Enter();
        // LogStackTrace(
        //	make_string("-S- Entered to csPlayers [%d]", GetCurrentThreadId()).c_str());
        VERIFY(!now_iterating_in_net_players);
        now_iterating_in_net_players = true;

        players_collection_t::iterator client_iter = std::find_if(net_Players.begin(), net_Players.end(), predicate);
        IClient* ret_client = nullptr;
        if (client_iter != net_Players.end())
        {
            ret_client = *client_iter;
            net_Players.erase(client_iter);
        }
        now_iterating_in_net_players = false;
        // Msg("-S- Leaving from csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Leave();
        return ret_client;
    }

    template <typename SearchPredicate>
    IClient* GetFoundClient(SearchPredicate const& predicate)
    {
        // Msg("-S- Entering to csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Enter();
        // LogStackTrace(
        //	make_string("-S- Entered to csPlayers [%d]", GetCurrentThreadId()).c_str());
        players_collection_t::iterator client_iter = std::find_if(net_Players.begin(), net_Players.end(), predicate);
        IClient* ret_client = nullptr;
        if (client_iter != net_Players.end())
        {
            ret_client = *client_iter;
        }
        // Msg("-S- Leaving from csPlayers [%d]", GetCurrentThreadId());
        csPlayers.Leave();
        return ret_client;
    }

    void AddNewClient(IClient* new_client)
    {
        
        csPlayers.Enter();

        VERIFY(!now_iterating_in_net_players);
        net_Players.push_back(new_client);
    
        csPlayers.Leave();
    }

    u32 ClientsCount()
    {
        csPlayers.Enter();
        u32 ret_count = net_Players.size();
        csPlayers.Leave();
        return ret_count;
    }

};

#endif 
