#include "KFGateModule.hpp"

namespace KFrame
{
    void KFGateModule::BeforeRun()
    {
        auto looptime = KFGlobal::Instance()->RandRange( 30000u, 40000u );
        __LOOP_TIMER_0__( looptime, 5000u, &KFGateModule::OnTimerUpdateOnlineToDir );

        __REGISTER_TCP_CLIENT_CONNECTION__( &KFGateModule::OnClientConnectionServer );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFGateModule::OnClientLostServer );
        __REGISTER_TCP_SERVER_LOST__( &KFGateModule::OnPlayerDisconnection );
        __REGISTER_TCP_SERVER_TRANSPOND__( &KFGateModule::TranspondToGame );
        __REGISTER_TCP_CLIENT_TRANSPOND__( &KFGateModule::TranspondToClient );

        __REGISTER_DEPLOY_FUNCTION__( __STRING__( shutdown ), &KFGateModule::OnDeployShutDownServer );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::MSG_LOGIN_REQ, &KFGateModule::HandleLoginReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::MSG_LOGOUT_REQ, &KFGateModule::HandleLogoutReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LOGIN_TO_GATE_ACK, &KFGateModule::HandleLoginToGateAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_RELOGIN_TO_GATE_ACK, &KFGateModule::HandleReLoginToGateAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ENTER_TO_GATE_ACK, &KFGateModule::HandleEnterToGateAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_KICK_PLAYER_TO_GATE_REQ, &KFGateModule::HandleKickPlayerToGateReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_GATE_REQ, &KFGateModule::HandleBroadcastToGateReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_SERVER_REQ, &KFGateModule::HandleBroadcastToServerReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFGateModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_TCP_CLIENT_SHUTDOWN__();
        __UN_TCP_CLIENT_CONNECTION__();
        __UN_TCP_SERVER_LOST__();
        __UN_TCP_SERVER_TRANSPOND__();
        __UN_TCP_CLIENT_TRANSPOND__();

        __UN_DEPLOY_FUNCTION__( __STRING__( shutdown ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_LOGIN_REQ );
        __UN_MESSAGE__( KFMsg::S2S_RELOGIN_TO_GATE_ACK );
        __UN_MESSAGE__( KFMsg::MSG_LOGOUT_REQ );
        __UN_MESSAGE__( KFMsg::S2S_LOGIN_TO_GATE_ACK );
        __UN_MESSAGE__( KFMsg::S2S_ENTER_TO_GATE_ACK );
        __UN_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GATE_REQ );
        __UN_MESSAGE__( KFMsg::S2S_BROADCAST_TO_SERVER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_GATE_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_DEPLOY_FUNCTION__( KFGateModule::OnDeployShutDownServer )
    {
        _is_server_shutdown = true;

        // 通知客户端服务器关系
        KFMsg::MsgTellShutDown tell;
        _kf_tcp_server->SendNetMessage( KFMsg::MSG_TELL_SHUTDOWN, &tell );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFGateModule::OnTimerUpdateOnlineToDir )
    {
        // 更新给dir服务器
        auto global = KFGlobal::Instance();
        _kf_dir_client->UpdateGateToDir( global->_app_id->GetId(), global->_internet_ip, global->_listen_port, _kf_role_list.Size(), 60u );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFGateModule::OnClientConnectionServer )
    {
        if ( net_data->_type == __STRING__( login ) )
        {
            _login_conhash.AddHashNode( net_data->_type, net_data->_id, 100 );
            _login_server_id = _login_conhash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
        }
        else if ( net_data->_type == __STRING__( game ) )
        {
            KFMsg::S2SConnectToGameReq req;
            for ( auto iter : _kf_role_list._objects )
            {
                auto kfrole = iter.second;
                if ( kfrole->_game_id == net_data->_id )
                {
                    auto pblogin = req.add_pblogin();
                    pblogin->set_token( kfrole->_token );
                    pblogin->set_playerid( kfrole->_id );
                    pblogin->set_accountid( kfrole->_account_id );
                    pblogin->set_sessionid( kfrole->_session_id );
                    pblogin->set_loginbycrash( true );
                    pblogin->set_gateid( KFGlobal::Instance()->_app_id->GetId() );
                }
            }
            _kf_tcp_client->SendNetMessage( net_data->_id, KFMsg::S2S_CONNECT_TO_GAME_REQ, &req );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFGateModule::OnClientLostServer )
    {
        if ( net_data->_type == __STRING__( login ) )
        {
            _login_conhash.RemoveHashNode( net_data->_id );
            if ( _login_server_id == net_data->_id )
            {
                _login_server_id = _login_conhash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_MAX_CLIENT_MSG_ID__ 10000
    __KF_FORWARD_MESSAGE_FUNCTION__( KFGateModule::TranspondToClient )
    {
        auto playerid = __ROUTE_RECV_ID__;
        if ( playerid == _invalid_int || msg_id >= __KF_MAX_CLIENT_MSG_ID__ )
        {
            return false;
        }

        auto kfrole = FindRole( playerid );
        if ( kfrole != nullptr )
        {
            kfrole->SendToClient( msg_id, data, length );
        }

        return true;
    }

    __KF_FORWARD_MESSAGE_FUNCTION__( KFGateModule::TranspondToGame )
    {
        auto playerid = __ROUTE_SEND_ID__;
        if ( playerid == _invalid_int || msg_id >= __KF_MAX_CLIENT_MSG_ID__ )
        {
            return false;
        }

        if ( _is_server_shutdown )
        {
            return true;
        }

        auto kfrole = FindRole( playerid );
        if ( kfrole != nullptr )
        {
            kfrole->SendToGame( msg_id, data, length );
        }

        return true;
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleBroadcastToGateReq, KFMsg::S2SBroadcastToGateReq )
    {
        if ( kfmsg->worldid() != _invalid_int )
        {
            // 检查是否重复广播
            auto iter = _broadcast_list.find( kfmsg->worldid() );
            if ( iter != _broadcast_list.end() )
            {
                if ( iter->second >= kfmsg->serial() )
                {
                    return;
                }
            }

            iter->second = kfmsg->serial();
        }

        auto& msgdata = kfmsg->msgdata();
        for ( auto& iter : _kf_role_list._objects )
        {
            auto kfrole = iter.second;
            kfrole->SendToClient( kfmsg->msg_id(), msgdata.data(), static_cast< uint32 >( msgdata.length() ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleBroadcastToServerReq, KFMsg::S2SBroadcastToServerReq )
    {
        auto& msgdata = kfmsg->msgdata();
        for ( auto& iter : _kf_role_list._objects )
        {
            auto kfrole = iter.second;
            if ( kfrole->_game_id == __ROUTE_SERVER_ID__ )
            {
                kfrole->SendToClient( kfmsg->msg_id(), msgdata.data(), static_cast< uint32 >( msgdata.length() ) );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGateModule::AddRole( KFRole* role )
    {
        RemoveRole( role->_id );

        _token_list[ role->_token ] = role;
        _kf_role_list.Insert( role->_id, role );
    }

    void KFGateModule::RemoveRole( uint64 id )
    {
        auto role = FindRole( id );
        if ( role == nullptr )
        {
            return;
        }

        RemoveRole( role );
    }

    void KFGateModule::RemoveRole( KFRole* role )
    {
        // 删除连接关系
        if ( role->_session_id != _invalid_int )
        {
            _kf_tcp_server->CloseNetHandle( role->_session_id, 1000, __FUNC_LINE__ );
        }

        _token_list.erase( role->_token );
        _kf_role_list.Remove( role->_id );
    }

    KFRole* KFGateModule::FindRole( uint64 id )
    {
        return _kf_role_list.Find( id );
    }

    KFRole* KFGateModule::FindRole( const std::string& token )
    {
        auto iter = _token_list.find( token );
        if ( iter == _token_list.end() )
        {
            return nullptr;
        }

        return iter->second;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGateModule::SendLoginAckMessage( uint64 sessionid, uint32 result, uint64 bantime )
    {
        // 消息到这里的都是错误结果
        _kf_display->SendToClient( sessionid, result, bantime );

        // 1秒后主动断开游戏
        _kf_tcp_server->CloseNetHandle( sessionid, 1000, __FUNC_LINE__ );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginReq, KFMsg::MsgLoginReq )
    {
        auto sessionid = __ROUTE_SERVER_ID__;

        auto& token = kfmsg->token();
        auto accountid = kfmsg->accountid();

        __LOG_DEBUG__( "session[{}] accountid[{}] version[{}] token[{}] login req", sessionid, accountid, kfmsg->version(), token );
        // 注册连接器
        if ( !_kf_tcp_server->RegisteNetHandle( sessionid, sessionid, _invalid_int ) )
        {
            return __LOG_ERROR__( "accountid[{}] register[{}] failed", accountid, sessionid );
        }

        // 消息是否有效
        if ( accountid == 0u || token.empty() )
        {
            return SendLoginAckMessage( sessionid, KFMsg::LoginTokenError, 0 );
        }

        // 判断版本号
        auto compatibility = KFGlobal::Instance()->CheckVersionCompatibility( kfmsg->version() );
        if ( !compatibility )
        {
            return SendLoginAckMessage( sessionid, KFMsg::VersionNotCompatibility, 0 );
        }

        // 服务器正在关闭中
        if ( _is_server_shutdown )
        {
            return SendLoginAckMessage( sessionid, KFMsg::LoginIsClose, 0 );
        }

        auto kfrole = FindRole( token );
        if ( kfrole != nullptr )
        {
            // 判断token
            if ( kfrole->_token != token || kfrole->_account_id != accountid )
            {
                return SendLoginAckMessage( sessionid, KFMsg::LoginTokenError, 0 );
            }

            // 设置连接成功
            __UN_TIMER_1__( kfrole->_id );
            kfrole->_session_id = sessionid;

            // 发送重新登录消息
            KFMsg::S2SReLoginToGameReq req;
            req.set_token( token );
            req.set_playerid( kfrole->_id );
            req.set_sessionid( sessionid );
            req.set_accountid( accountid );
            auto ok = kfrole->SendToGame( KFMsg::S2S_RELOGIN_TO_GAME_REQ, &req );
            if ( !ok )
            {
                __LOG_ERROR__( "session[{}] accountid[{}] relogin failed", sessionid, accountid );

                // 发送错误
                SendLoginAckMessage( sessionid, KFMsg::LoginSystemBusy, 0 );
            }
        }
        else
        {
            LoginToLogin( sessionid, accountid, token );
        }
    }

    void KFGateModule::LoginToLogin( uint64 sessionid, uint64 accountid, const std::string& token )
    {
        // 没有可用的login
        if ( _login_server_id == _invalid_int )
        {
            return SendLoginAckMessage( sessionid, KFMsg::LoginNoLoginServer, 0 );
        }

        // ip
        auto& ip = _kf_tcp_server->GetHandleIp( sessionid );

        // 发送到Login服务器验证
        KFMsg::S2SLoginToLoginReq req;
        req.set_ip( ip );
        req.set_token( token );
        req.set_accountid( accountid );
        req.set_sessionid( sessionid );
        auto ok = _kf_tcp_client->SendNetMessage( _login_server_id, KFMsg::S2S_LOGIN_TO_LOGIN_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "session[{}] accountid[{}] send login failed", sessionid, accountid );

            // 发送错误
            SendLoginAckMessage( sessionid, KFMsg::LoginSystemBusy, 0 );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginToGateAck, KFMsg::S2SLoginToGateAck )
    {
        __LOG_DEBUG__( "player[{}] login result[{}]", kfmsg->accountid(), kfmsg->result() );
        SendLoginAckMessage( kfmsg->sessionid(), kfmsg->result(), kfmsg->bantime() );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleReLoginToGateAck, KFMsg::S2SReLoginToGateAck )
    {

        __LOG_DEBUG__( "player[{}:{}] relogin ack", kfmsg->accountid(), kfmsg->playerid() );

        // game server上没有角色了, 重新登录
        LoginToLogin( kfmsg->sessionid(), kfmsg->accountid(), kfmsg->token() );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleEnterToGateAck, KFMsg::S2SEnterToGateAck )
    {
        auto pblogin = &kfmsg->pblogin();
        __LOG_DEBUG__( "player[{}:{}] session[{}] enter game", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );

        auto kfrole = FindRole( pblogin->playerid() );
        if ( kfrole != nullptr )
        {
            // 如果已经掉线
            if ( kfrole->_session_id == _invalid_int )
            {
                return __LOG_DEBUG__( "player=[{}] disconnect", pblogin->playerid() );
            }

            // 已经在线, 应该是Game宕机后重新加载的逻辑
            kfrole->_game_id = __ROUTE_SERVER_ID__;
        }
        else
        {
            // 绑定角色id
            if ( !_kf_tcp_server->BindObjectId( pblogin->sessionid(), pblogin->playerid() ) )
            {
                // 发送消息给game, 踢掉玩家
                KFMsg::S2SLeaveToGameReq req;
                req.set_playerid( pblogin->playerid() );
                _kf_tcp_client->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_LEAVE_TO_GAME_REQ, &req );
                return __LOG_ERROR__( "player[{}:{}] session[{}] failed", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );
            }

            // 删除掉线定时器
            __UN_TIMER_1__( pblogin->playerid() );

            // 创建角色
            kfrole = __KF_NEW__( KFRole );
            kfrole->_id = pblogin->playerid();
            kfrole->_token = pblogin->token();
            kfrole->_game_id = __ROUTE_SERVER_ID__;
            kfrole->_session_id = pblogin->sessionid();
            kfrole->_account_id = pblogin->accountid();
            AddRole( kfrole );
        }

        // 通知进入游戏
        KFMsg::MsgLoginAck ack;
        ack.set_playerid( pblogin->playerid() );
        ack.set_servertime( kfmsg->servertime() );
        ack.mutable_playerdata()->CopyFrom( kfmsg->playerdata() );
        auto ok = kfrole->SendToClient( KFMsg::MSG_LOGIN_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] session[{}] enter failed", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFGateModule::OnPlayerDisconnection )
    {
        auto kfrole = FindRole( net_data->_id );
        if ( kfrole == nullptr )
        {
            return;
        }

        __LOG_DEBUG__( "client[{}] disconnection", net_data->_id );

        KFMsg::S2SDisconnectToGameReq req;
        req.set_playerid( kfrole->_id );
        kfrole->SendToGame( KFMsg::S2S_DISCONNECT_TO_GAME_REQ, &req );

        // 添加一个定时器
        kfrole->_session_id = _invalid_int;

        static auto _disconnect_time_constant = KFGlobal::Instance()->FindConstant( __STRING__( disconnecttime ) );
        __LIMIT_TIMER_1__( kfrole->_id, _disconnect_time_constant->_uint32_value, 1, &KFGateModule::OnTimerPlayerDisconnetion );
    }

    __KF_TIMER_FUNCTION__( KFGateModule::OnTimerPlayerDisconnetion )
    {
        auto kfrole = FindRole( objectid );
        if ( kfrole == nullptr )
        {
            return;
        }

        KFMsg::S2SLeaveToGameReq req;
        req.set_playerid( objectid );
        kfrole->SendToGame( KFMsg::S2S_LEAVE_TO_GAME_REQ, &req );

        // 删除玩家
        RemoveRole( kfrole );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleKickPlayerToGateReq, KFMsg::S2SKickPlayerToGateReq )
    {
        auto kfrole = FindRole( kfmsg->playerid() );
        if ( kfrole == nullptr )
        {
            return;
        }

        // 通知客户端你被踢了
        KFMsg::MsgTellBeKick kick;
        kick.set_type( kfmsg->type() );
        kfrole->SendToClient( KFMsg::MSG_TELL_BE_KICK, &kick );

        // 删除玩家
        RemoveRole( kfrole );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLogoutReq, KFMsg::MsgLogoutReq )
    {
        auto playerid = __ROUTE_SEND_ID__;
        auto kfrole = FindRole( playerid );
        if ( kfrole != nullptr )
        {
            __LOG_DEBUG__( "player[{}] login out", playerid );

            KFMsg::S2SLogoutToGameReq req;
            req.set_playerid( playerid );
            kfrole->SendToGame( KFMsg::S2S_LOGOUT_TO_GAME_REQ, &req );

            // 删除玩家
            RemoveRole( kfrole );
        }
        else
        {
            // 找不到直接关闭
            _kf_tcp_server->CloseNetHandle( __ROUTE_SERVER_ID__, 1000, __FUNC_LINE__ );
        }

        KFMsg::MsgLogoutAck ack;
        _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::MSG_LOGOUT_ACK, &ack );
    }

    //// 走马灯
    //__KF_COMMAND_FUNCTION__( OnCommandMarquee );

    //// 系统公告
    //__KF_COMMAND_FUNCTION__( OnCommandNotice );

    //__REGISTER_COMMAND_FUNCTION__( __STRING__( notice ), &KFWorldModule::OnCommandNotice );
    //__REGISTER_COMMAND_FUNCTION__( __STRING__( marquee ), &KFWorldModule::OnCommandMarquee );

    //__UN_COMMAND_FUNCTION__( __STRING__( notice ) );
    //__UN_COMMAND_FUNCTION__( __STRING__( marquee ) );
    //__KF_COMMAND_FUNCTION__( KFWorldModule::OnCommandMarquee )
    //{
    //    KFMsg::MsgTellMarquee tell;
    //    tell.set_content( param );
    //    BroadcastMessageToGame( KFMsg::MSG_TELL_MARQUEE, &tell );
    //}

    //__KF_COMMAND_FUNCTION__( KFWorldModule::OnCommandNotice )
    //{
    //    KFMsg::MsgTellSysNotcie tell;
    //    tell.set_content( param );
    //    BroadcastMessageToGame( KFMsg::MSG_TELL_SYS_NOTICE, &tell );
    //}
}
