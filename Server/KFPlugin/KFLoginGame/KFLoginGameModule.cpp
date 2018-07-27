#include "KFLoginGameModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFLoginGameModule::KFLoginGameModule()
    {


    }

    KFLoginGameModule::~KFLoginGameModule()
    {

    }

    void KFLoginGameModule::InitModule()
    {
    }

    void KFLoginGameModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFLoginGameModule::OnServerLostHandle );
        __REGISTER_LOOP_TIMER__( 0, 10000, &KFLoginGameModule::OnTimerUpdateLoginGameToWorld );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_PROXY_UPDATE_REQ, &KFLoginGameModule::HandleLoginProxyUpdateReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_REQ, &KFLoginGameModule::HandleLoginGameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ, &KFLoginGameModule::HandleLoginOutReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK, &KFLoginGameModule::HandleLoadPlayerAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_REQ, &KFLoginGameModule::HandleLoginTellTokenToGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_ACK, &KFLoginGameModule::HandleLoginTellTokenToGameAck );
    }

    void KFLoginGameModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_PROXY_UPDATE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFLoginGameModule::OnTimerUpdateLoginGameToWorld )
    {
        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SLoginGameUpdateReq req;
        req.set_gameid( kfglobal->_app_id );
        req.set_count( _kf_player->GetPlayerCount() );
        _kf_game->SendMessageToWorld( KFMsg::S2S_LOGIN_GAME_UPDATE_REQ, &req );
    }

    __KF_SERVER_LOST_FUNCTION__( KFLoginGameModule::OnServerLostHandle )
    {
        if ( handletype == __KF_STRING__( proxy ) )
        {
            // 更新到world
            KFMsg::S2SLoginProxyRemoveReq req;
            req.set_proxyid( handleid );
            _kf_game->SendMessageToWorld( KFMsg::S2S_LOGIN_PROXY_REMOVE_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginGameModule::HandleLoginProxyUpdateReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginProxyUpdateReq );

        // 直接转发到word
        _kf_game->SendMessageToWorld( KFMsg::S2S_LOGIN_PROXY_UPDATE_REQ, &kfmsg );
    }

    __KF_MESSAGE_FUNCTION__( KFLoginGameModule::HandleLoginTellTokenToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginTellTokenToGameReq );

        KFMsg::S2SLoginTellTokenToProxyReq req;
        req.set_playerid( kfmsg.playerid() );
        req.set_token( kfmsg.token() );
        req.set_gateid( kfmsg.gateid() );
        req.set_loginid( kfmsg.loginid() );
        req.set_accountid( kfmsg.accountid() );
        auto ok = _kf_game->SendMessageToGate( kfmsg.proxyid(), KFMsg::S2S_LOGIN_TELL_TOKEN_TO_PROXY_REQ, &req );
        if ( ok )
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] token to proxy ok!",
                                __FUNCTION__, kfmsg.accountid(), kfmsg.playerid() );
        }
        else
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] token to proxy failed!",
                                __FUNCTION__, kfmsg.accountid(), kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginGameModule::HandleLoginTellTokenToGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginTellTokenToGameAck );

        KFMsg::S2SLoginTellTokenToWorldAck ack;
        ack.set_ip( kfmsg.ip() );
        ack.set_port( kfmsg.port() );
        ack.set_token( kfmsg.token() );
        ack.set_gateid( kfmsg.gateid() );
        ack.set_loginid( kfmsg.loginid() );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_accountid( kfmsg.accountid() );
        auto ok = _kf_game->SendMessageToWorld( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_WORLD_ACK, &ack );
        if ( ok )
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] token to world ok!",
                                __FUNCTION__, kfmsg.accountid(), kfmsg.playerid() );
        }
        else
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] token to world failed!",
                                __FUNCTION__, kfmsg.accountid(), kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginGameModule::HandleLoginGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginGameReq );

        auto gateid = __KF_HEAD_ID__( kfguid );
        uint32 playerid = kfmsg.playerid();
        KFLogger::LogLogin( KFLogger::Info, "[%s] player[%u] login game req!",
                            __FUNCTION__, playerid );

        // 踢掉在线玩家
        _kf_player->KickPlayer( playerid, KFMsg::KickEnum::LoginBeKick, __FUNCTION__, _invalid_int );

        auto zoneid = KFUtility::CalcZoneId( playerid );

        // 加载玩家数据
        KFMsg::S2SLoginLoadPlayerReq req;
        req.set_playerid( playerid );
        req.set_gateid( gateid );
        req.set_zoneid( zoneid );
        req.set_accountid( kfmsg.accountid() );
        auto ok = _kf_data->SendMessageToData( zoneid, KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ, &req );
        if ( ok )
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] player[%u] login game ok!",
                                __FUNCTION__, playerid );
        }
        else
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] player[%u] login game failed!",
                                __FUNCTION__, playerid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginGameModule::HandleLoadPlayerAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoadPlayerAck );

        auto gateid = kfmsg.gateid();
        auto playerid = kfmsg.playerid();
        auto result = kfmsg.result();
        auto playerdata = kfmsg.mutable_playerdata();
        auto extenddata = kfmsg.mutable_extenddata();

        KFLogger::LogLogin( KFLogger::Info, "[%s] player[%u] load data ack!",
                            __FUNCTION__, playerid );

        if ( result != KFMsg::Success )
        {
            return SendLoginGameMessage( result, playerid, gateid, playerdata );
        }

        // 创建玩家
        auto player = _kf_player->CreatePlayer( gateid, playerid, playerdata, extenddata );

        // 同步给客户端
        _kf_kernel->SerializeToOnline( player->GetData(), playerdata );
        SendLoginGameMessage( result, playerid, gateid, playerdata );
    }

    void KFLoginGameModule::SendLoginGameMessage( uint32 result, uint32 playerid, uint32 gateid, const KFMsg::PBObject* playerdata )
    {
        KFMsg::S2SLoginGameAck ack;
        ack.set_result( result );
        ack.set_playerid( playerid );
        ack.mutable_playerdata()->CopyFrom( *playerdata );
        ack.set_servertime( KFGlobal::Instance()->_real_time );
        auto ok = _kf_game->SendMessageToGate( gateid, KFMsg::S2S_LOGIN_GAME_ACK, &ack );
        if ( ok )
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] player[%u] load game result[%u] ok!",
                                __FUNCTION__, playerid, result );
        }
        else
        {
            KFLogger::LogLogin( KFLogger::Error, "[%s] player[%u] load game result[%u] failed!",
                                __FUNCTION__, playerid, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginGameModule::HandleLoginOutReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginOutReq );

        // 玩家登出
        _kf_player->LogoutPlayer( kfmsg.playerid() );
    }

}