#include "KFDisplayModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    void KFDisplayModule::SendToClient( uint64 playerid, uint32 result, ListString& params )
    {
        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }

        _kf_tcp_server->SendNetMessage( playerid, KFMsg::MSG_RESULT_DISPLAY, &display );
    }

    void KFDisplayModule::SendToClient( KFEntity* player, uint32 result, ListString& params )
    {
        if ( _kf_game == nullptr )
        {
            return;
        }

        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }

        _kf_game->SendToClient( player, KFMsg::MSG_RESULT_DISPLAY, &display );
    }

    void KFDisplayModule::SendToPlayer( uint64 serverid, uint64 playerid, uint32 result, ListString& params )
    {
        if ( _kf_route == nullptr )
        {
            return;
        }

        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }
        _kf_route->SendToPlayer( 0u, serverid, playerid, KFMsg::MSG_RESULT_DISPLAY, &display );
    }

    void KFDisplayModule::SendToPlayer( KFData* kfbasic, uint32 result, ListString& params )
    {
        if ( _kf_game == nullptr )
        {
            return;
        }

        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }
        _kf_game->SendToPlayer( 0u, kfbasic, KFMsg::MSG_RESULT_DISPLAY, &display );
    }

    void KFDisplayModule::SendToPlayer( const Route& route, uint32 result, ListString& params )
    {
        if ( _kf_route == nullptr )
        {
            return;
        }

        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }
        _kf_route->SendToRoute( route, KFMsg::MSG_RESULT_DISPLAY, &display );
    }


    void KFDisplayModule::SendToGroup( KFEntity* player, uint32 result, ListString& params )
    {
        //KFMsg::MsgResultDisplay display;
        //display.set_result( result );
        //for ( auto& param : params )
        //{
        //    display.add_param( param );
        //}

        //if ( _kf_player != nullptr )
        //{
        //    _kf_player->SendToGroup( player, KFMsg::MSG_RESULT_DISPLAY, &display );
        //}
        //else
        //{
        //    __LOG_ERROR__( "_kf_player is nullptr!" );
        //}
    }
}