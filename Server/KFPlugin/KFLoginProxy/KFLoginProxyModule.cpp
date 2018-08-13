#include "KFLoginProxyModule.h"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    KFLoginProxyModule::KFLoginProxyModule()
    {
    }

    KFLoginProxyModule::~KFLoginProxyModule()
    {
    }

    void KFLoginProxyModule::InitModule()
    {
    }

    void KFLoginProxyModule::BeforeRun()
    {
        __REGISTER_RUN_FUNCTION__( &KFLoginProxyModule::Run );
        __REGISTER_AFTER_RUN_FUNCTION__( &KFLoginProxyModule::AfterRun );
        __REGISTER_LOOP_TIMER__( 0, 10000, &KFLoginProxyModule::OnTimerUpdateOnlineToGame );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_GAME_REQ, &KFLoginProxyModule::HandleLoginGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_ACK, &KFLoginProxyModule::HandleLoginGameAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ, &KFLoginProxyModule::HandleLoginOutReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_PROXY_REQ, &KFLoginProxyModule::HandleLoginTellTokenToProxyReq );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFLoginProxyModule::ShutDown()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_RUN_FUNCTION__();
        __UNREGISTER_AFTER_RUN_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_PROXY_REQ );
    }

    void KFLoginProxyModule::Run()
    {
        // 检查token的有效时间
        RunLoginTokenValidTime();
    }

    void KFLoginProxyModule::AfterRun()
    {
        RunRemoveLoginToken();
    }

    void KFLoginProxyModule::RunLoginTokenValidTime()
    {
        auto nowtime = KFGlobal::Instance()->_real_time;
        for ( auto& iter : _kf_token_list._objects )
        {
            auto kftoken = iter.second;
            if ( kftoken->_valid_time < nowtime )
            {
                _kf_remove_list.insert( kftoken->_player_id );
            }
        }
    }

    void KFLoginProxyModule::RunRemoveLoginToken()
    {
        if ( _kf_remove_list.empty() )
        {
            return;
        }

        for ( auto playerid : _kf_remove_list )
        {
            _kf_token_list.Remove( playerid );
        }

        _kf_remove_list.clear();
    }

    __KF_TIMER_FUNCTION__( KFLoginProxyModule::OnTimerUpdateOnlineToGame )
    {
        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SLoginProxyUpdateReq req;
        req.set_proxyid( kfglobal->_app_id );
        req.set_ip( kfglobal->_interanet_ip );
        req.set_port( kfglobal->_listen_port );
        req.set_count( _kf_tcp_server->GetHandleCount() );
        _kf_gate->SendMessageToGame( KFMsg::S2S_LOGIN_PROXY_UPDATE_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLoginProxyModule::HandleLoginTellTokenToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginTellTokenToProxyReq );

        auto kftoken = _kf_token_list.Create( kfmsg.playerid() );
        kftoken->_token = kfmsg.token();
        kftoken->_player_id = kfmsg.playerid();
        kftoken->_game_id = __KF_HEAD_ID__( kfguid );
        kftoken->_account_id = kfmsg.accountid();
        kftoken->_valid_time = KFGlobal::Instance()->_real_time + 120;

        KFMsg::S2SLoginTellTokenToGameAck ack;
        ack.set_token( kfmsg.token() );
        ack.set_gateid( kfmsg.gateid() );
        ack.set_loginid( kfmsg.loginid() );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_accountid( kfmsg.accountid() );
        ack.set_ip( KFGlobal::Instance()->_interanet_ip );
        ack.set_port( KFGlobal::Instance()->_listen_port );
        auto ok = _kf_gate->SendMessageToGame( __KF_HEAD_ID__( kfguid ), KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_ACK, &ack );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Login, "player[{}:{}] token to game ok!", kfmsg.accountid(), kfmsg.playerid() );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Login, "Player[{}:{}] token to game failed!", kfmsg.accountid(), kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginProxyModule::HandleLoginGameReq )
    {
        __PROTO_PARSE__( KFMsg::MsgLoginGameReq );
        auto hendleid = __KF_HEAD_ID__( kfguid );
        auto playerid = kfmsg.playerid();
        __LOG_DEBUG__( KFLogEnum::Login, "player[{}] login proxy req!", playerid );

        auto kftoken = _kf_token_list.Find( playerid );
        if ( kftoken == nullptr )
        {
            return __LOG_ERROR__( KFLogEnum::Login, "player[{}] can't find token!", playerid );
        }

        if ( kftoken->_token != kfmsg.token() )
        {
            return __LOG_ERROR__( KFLogEnum::Login, "player[{}] token error!", playerid );
        }

        auto gameid = kftoken->_game_id;
        auto accountid = kftoken->_account_id;

        // 删除token
        _kf_token_list.Remove( playerid );

        // 注册网络
        auto ok = _kf_gate->AddConnection( hendleid, playerid );
        if ( !ok )
        {
            _kf_gate->RemoveConnection( playerid, 100, __FUNC_LINE__ );
            return __LOG_ERROR__( KFLogEnum::Login, "player[{}] register connection failed!", playerid );
        }

        // 创建一个Proxy对象
        auto kfproxy = _kf_proxy->CreateProxy( playerid );
        kfproxy->SetGameId( gameid );
        kfproxy->SetPlayerId( playerid );

        auto& ip = _kf_gate->GetIp( playerid );

        // 发送给gameserver, 请求登录
        KFMsg::S2SLoginGameReq req;
        req.set_playerid( playerid );
        req.set_accountid( accountid );
        req.set_ip( ip );
        ok = kfproxy->SendMessageToGame( KFMsg::S2S_LOGIN_GAME_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Login, "player[{}:{}] login proxy ok!", playerid, ip );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Login, "player[{}:{}] login proxy failed!", playerid, ip );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginProxyModule::HandleLoginGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginGameAck );
        auto result = kfmsg.result();
        auto playerid = kfmsg.playerid();

        if ( result != KFMsg::Success )
        {
            __LOG_ERROR__( KFLogEnum::Login, "player[{}] login failed result[{}]!", playerid, result );

            // 发送错误消息
            _kf_display->SendToClient( playerid, result );

            // 断开连接, 客户端重新走登录流程
            _kf_gate->RemoveConnection( playerid, 1000, __FUNC_LINE__ );
        }
        else
        {
            auto kfproxy = _kf_proxy->FindProxy( playerid );
            if ( kfproxy == nullptr )
            {
                return __LOG_ERROR__( KFLogEnum::Login, "can not find proxy[{}]", playerid );
            }

            KFMsg::MsgEnterGame enter;
            enter.set_servertime( kfmsg.servertime() );
            enter.mutable_playerdata()->CopyFrom( kfmsg.playerdata() );
            auto ok = kfproxy->SendMessageToClient( KFMsg::MSG_LOGIN_ENTER_GAME, &enter );
            if ( ok )
            {
                __LOG_DEBUG__( KFLogEnum::Login, "player[{}] enter game ok!", playerid );
            }
            else
            {
                __LOG_ERROR__( KFLogEnum::Login, "player[{}] enter game failed!", playerid );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginProxyModule::HandleLoginOutReq )
    {
        auto playerid = __KF_HEAD_ID__( kfguid );
        __LOG_DEBUG__( KFLogEnum::Login, "player[{}] login out!", playerid );

        KFMsg::S2SLoginOutReq req;
        req.set_playerid( playerid );
        auto kfproxy = _kf_proxy->FindProxy( playerid );
        if ( kfproxy != nullptr )
        {
            kfproxy->SendMessageToGame( KFMsg::S2S_LOGIN_OUT_REQ, &req );

            // 删除玩家
            _kf_proxy->RemoveProxy( playerid );
        }

        // 断开链接
        _kf_gate->RemoveConnection( playerid, 1000, __FUNC_LINE__ );
    }

}