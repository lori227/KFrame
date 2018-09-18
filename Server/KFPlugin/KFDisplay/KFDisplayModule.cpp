#include "KFDisplayModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFDisplayModule::KFDisplayModule()
    {
    }

    KFDisplayModule::~KFDisplayModule()
    {

    }
    void KFDisplayModule::InitModule()
    {
    }

    void KFDisplayModule::BeforeRun()
    {
    }

    void KFDisplayModule::BeforeShut()
    {
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    void KFDisplayModule::SendToClient( uint32 playerid, uint32 result, ListString& params )
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
            _kf_player->SendMessageToClient( player, KFMsg::MSG_RESULT_DISPLAY, &display );
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
            _kf_player->SendMessageToGroup( player, KFMsg::MSG_RESULT_DISPLAY, &display );
        }
        else
        {
            __LOG_ERROR__( "_kf_player is nullptr!" );
        }
    }

    void KFDisplayModule::SendToGame( uint32 serverid, uint32 playerid, uint32 result, ListString& params )
    {
        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }

        if ( _kf_cluster_shard != nullptr )
        {
            _kf_cluster_shard->SendMessageToPlayer( serverid, playerid, KFMsg::MSG_RESULT_DISPLAY, &display );
        }
        else
        {
            __LOG_ERROR__( "_kf_cluster_shard is nullptr!" );
        }
    }

    void KFDisplayModule::SendToPlayer( uint32 serverid, uint32 playerid, uint32 result, ListString& params )
    {
        KFMsg::MsgResultDisplay display;
        display.set_result( result );
        for ( auto& param : params )
        {
            display.add_param( param );
        }

        if ( _kf_route != nullptr )
        {
            _kf_route->SendMessageToRoute( serverid, playerid, KFMsg::MSG_RESULT_DISPLAY, &display );
        }
        else
        {
            __LOG_ERROR__( "_kf_route is nullptr!" );
        }
    }

    void KFDisplayModule::SendToPlayer( KFData* kfbasic, uint32 result, ListString& params )
    {
        auto serverid = kfbasic->GetValue< uint32 >( __KF_STRING__( serverid ) );
        auto playerid = kfbasic->GetValue< uint32 >( __KF_STRING__( id ) );
        SendToPlayer( serverid, playerid, result, params );
    }
}