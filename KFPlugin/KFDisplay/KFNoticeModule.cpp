#include "KFNoticeModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{

#define __NOTICE_MESSAGE__()\
    KFMsg::MsgTellNotice tell;\
    auto pbnotice = tell.add_notice();\
    pbnotice->set_type( type );\
    pbnotice->set_count( count );\

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    void KFNoticeModule::SendToPlayer( KFEntity* player, uint32 type, uint32 count )
    {
        if ( _kf_game == nullptr || !player->IsInited() )
        {
            return;
        }

        __NOTICE_MESSAGE__();
        _kf_game->SendToClient( player, KFMsg::MSG_TELL_NOTICE, &tell );
    }

    void KFNoticeModule::SendToPlayer( KFData* kfbasic, uint32 type, uint32 count )
    {
        if ( _kf_game == nullptr )
        {
            return;
        }

        __NOTICE_MESSAGE__();
        _kf_game->SendToPlayer( 0u, kfbasic, KFMsg::MSG_TELL_NOTICE, &tell );
    }

    void KFNoticeModule::SendToPlayer( uint64 serverid, uint64 playerid, uint32 type, uint32 count )
    {
        if ( _kf_route == nullptr )
        {
            return;
        }

        __NOTICE_MESSAGE__();
        _kf_route->RepeatToPlayer( 0u, serverid, playerid, KFMsg::MSG_TELL_NOTICE, &tell );
    }

    void KFNoticeModule::SendToPlayer( const Route& route, uint32 type, uint32 count )
    {
        if ( _kf_route == nullptr )
        {
            return;
        }

        __NOTICE_MESSAGE__();
        _kf_route->SendToRoute( route, KFMsg::MSG_TELL_NOTICE, &tell );
    }
}