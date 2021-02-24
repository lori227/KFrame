#include "KFDisplayModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __DISPLAY_MESSAGE__()\
    KFMsg::MsgResultDisplay display;\
    display.set_result( result );\
    for ( auto& param : params )\
    {\
        display.add_param( param );\
    }\

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    void KFDisplayModule::SendToClient( uint64 player_id, uint32 result, StringList& params )
    {
        if ( _kf_tcp_server == nullptr )
        {
            return;
        }

        __DISPLAY_MESSAGE__();
        _kf_tcp_server->SendNetMessage( player_id, KFMsg::MSG_RESULT_DISPLAY, &display );
    }

    void KFDisplayModule::SendToClient( EntityPtr player, uint32 result, StringList& params )
    {
        if ( _kf_game == nullptr || !player->IsInited() )
        {
            return;
        }

        __DISPLAY_MESSAGE__();
        _kf_game->SendToClient( player, KFMsg::MSG_RESULT_DISPLAY, &display );
    }

    void KFDisplayModule::DelayToClient( EntityPtr player, uint32 result, StringList& params )
    {
        if ( _kf_game == nullptr || !player->IsInited() )
        {
            return;
        }

        __DISPLAY_MESSAGE__();
        _kf_game->SendToClient( player, KFMsg::MSG_RESULT_DISPLAY, &display, 1u );
    }

    void KFDisplayModule::SendToPlayer( uint64 server_id, uint64 player_id, uint32 result, StringList& params )
    {
        if ( _kf_route == nullptr )
        {
            return;
        }

        __DISPLAY_MESSAGE__();
        _kf_route->RepeatToEntity( 0u, server_id, player_id, KFMsg::MSG_RESULT_DISPLAY, &display );
    }

    void KFDisplayModule::SendToPlayer( DataPtr kfbasic, uint32 result, StringList& params )
    {
        if ( _kf_game == nullptr )
        {
            return;
        }

        __DISPLAY_MESSAGE__();
        _kf_game->SendToPlayer( 0u, kfbasic, KFMsg::MSG_RESULT_DISPLAY, &display );
    }

    void KFDisplayModule::SendToPlayer( const Route& route, uint32 result, StringList& params )
    {
        if ( _kf_route == nullptr )
        {
            return;
        }

        __DISPLAY_MESSAGE__();
        _kf_route->SendToRoute( route, KFMsg::MSG_RESULT_DISPLAY, &display );
    }


    void KFDisplayModule::SendToGroup( EntityPtr player, uint32 result, StringList& params )
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
        //    __LOG_ERROR__( "_kf_player is nullptr" );
        //}
    }
}