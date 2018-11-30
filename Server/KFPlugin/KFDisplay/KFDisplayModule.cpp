#include "KFDisplayModule.h"
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
        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }

        if ( _kf_player != nullptr )
        {
            _kf_player->SendToClient( player, KFMsg::MSG_RESULT_DISPLAY, &display );
        }
        else
        {
            __LOG_ERROR__( "_kf_player is nullptr!" );
        }
    }

    void KFDisplayModule::SendToGroup( KFEntity* player, uint32 result, ListString& params )
    {
        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }

        if ( _kf_player != nullptr )
        {
            _kf_player->SendToGroup( player, KFMsg::MSG_RESULT_DISPLAY, &display );
        }
        else
        {
            __LOG_ERROR__( "_kf_player is nullptr!" );
        }
    }

    void KFDisplayModule::SendToGame( uint64 serverid, uint64 playerid, uint32 result, ListString& params )
    {
        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }

        if ( _kf_cluster_shard != nullptr )
        {
            _kf_cluster_shard->SendToPlayer( serverid, playerid, KFMsg::MSG_RESULT_DISPLAY, &display );
        }
        else
        {
            __LOG_ERROR__( "_kf_cluster_shard is nullptr!" );
        }
    }

    void KFDisplayModule::SendToPlayer( uint64 serverid, uint64 playerid, uint32 result, ListString& params )
    {
        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }

        if ( _kf_route != nullptr )
        {
            _kf_route->SendToRoute( serverid, playerid, KFMsg::MSG_RESULT_DISPLAY, &display );
        }
        else
        {
            __LOG_ERROR__( "_kf_route is nullptr!" );
        }
    }

    void KFDisplayModule::SendToPlayer( KFData* kfbasic, uint32 result, ListString& params )
    {
        auto serverid = kfbasic->GetValue( __KF_STRING__( serverid ) );
        auto playerid = kfbasic->GetValue( __KF_STRING__( id ) );
        SendToPlayer( serverid, playerid, result, params );
    }
}