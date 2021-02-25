#include "KFGateModule.hpp"

namespace KFrame
{
    void KFGateModule::BeforeRun()
    {
        auto loop_time = KFGlobal::Instance()->RandRange( 30000u, 40000u );
        __LOOP_TIMER_0__( loop_time, 5000u, &KFGateModule::OnTimerUpdateOnlineToDir );

        __REGISTER_TCP_CLIENT_CONNECTION__( &KFGateModule::OnClientConnectionServer );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFGateModule::OnClientLostServer );
        __REGISTER_TCP_SERVER_LOST__( &KFGateModule::OnPlayerDisconnection );
        __REGISTER_TCP_SERVER_FORWARD__( &KFGateModule::TranspondToGame );
        __REGISTER_TCP_CLIENT_FORWARD__( &KFGateModule::ForwardToClient );

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
        __UN_TCP_SERVER_FORWARD__();
        __UN_TCP_CLIENT_FORWARD__();

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
        _kf_dir_client->UpdateGateToDir( global->_app_id->GetId(), global->_internet_ip, global->_listen_port, _role_list.Size(), 60u );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFGateModule::OnClientConnectionServer )
    {
        if ( net_data->_type == __STRING__( login ) )
        {
            _login_hash.AddHashNode( net_data->_type, net_data->_id, 100 );
            _login_server_id = _login_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
        }
        else if ( net_data->_type == __STRING__( game ) )
        {
            KFMsg::S2SConnectToGameReq req;
            for ( auto iter : _role_list._objects )
            {
                auto role = iter.second;
                if ( role->_game_id == net_data->_id )
                {
                    auto login_data = req.add_pblogin();
                    login_data->set_token( role->_token );
                    login_data->set_playerid( role->_id );
                    login_data->set_accountid( role->_account_id );
                    login_data->set_sessionid( role->_session_id );
                    login_data->set_loginbycrash( true );
                    login_data->set_gateid( KFGlobal::Instance()->_app_id->GetId() );
                }
            }
            _kf_tcp_client->SendNetMessage( net_data->_id, KFMsg::S2S_CONNECT_TO_GAME_REQ, &req );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFGateModule::OnClientLostServer )
    {
        if ( net_data->_type == __STRING__( login ) )
        {
            _login_hash.RemoveHashNode( net_data->_id );
            if ( _login_server_id == net_data->_id )
            {
                _login_server_id = _login_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_MAX_CLIENT_MSG_ID__ 10000
    __KF_FORWARD_MESSAGE_FUNCTION__( KFGateModule::ForwardToClient )
    {
        auto player_id = __ROUTE_RECV_ID__;
        if ( player_id == _invalid_int || msg_id >= __KF_MAX_CLIENT_MSG_ID__ )
        {
            return false;
        }

        auto role = FindRole( player_id );
        if ( role != nullptr )
        {
            role->SendToClient( msg_id, data, length );
        }

        return true;
    }

    __KF_FORWARD_MESSAGE_FUNCTION__( KFGateModule::TranspondToGame )
    {
        auto player_id = __ROUTE_SEND_ID__;
        if ( player_id == _invalid_int || msg_id >= __KF_MAX_CLIENT_MSG_ID__ )
        {
            return false;
        }

        if ( _is_server_shutdown )
        {
            return true;
        }

        auto role = FindRole( player_id );
        if ( role != nullptr )
        {
            role->SendToGame( msg_id, data, length );
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

        auto& msg_data = kfmsg->msgdata();
        for ( auto& iter : _role_list._objects )
        {
            auto role = iter.second;
            role->SendToClient( kfmsg->msgid(), msg_data.data(), static_cast<uint32>( msg_data.length() ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleBroadcastToServerReq, KFMsg::S2SBroadcastToServerReq )
    {
        auto& msg_data = kfmsg->msgdata();
        for ( auto& iter : _role_list._objects )
        {
            auto role = iter.second;
            if ( role->_game_id == __ROUTE_SERVER_ID__ )
            {
                role->SendToClient( kfmsg->msgid(), msg_data.data(), static_cast<uint32>( msg_data.length() ) );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGateModule::AddRole( std::shared_ptr<KFRole> role )
    {
        RemoveRole( role->_id );

        _token_list.Insert( role->_ip, role );
        _role_list.Insert( role->_id, role );
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

    void KFGateModule::RemoveRole( std::shared_ptr<KFRole> role )
    {
        // 删除连接关系
        if ( role->_session_id != _invalid_int )
        {
            _kf_tcp_server->CloseNetHandle( role->_session_id, 1000, __FUNC_LINE__ );
        }

        _role_list.Remove( role->_id );
        _token_list.Remove( role->_token );
    }

    std::shared_ptr<KFRole> KFGateModule::FindRole( uint64 id )
    {
        return _role_list.Find( id );
    }

    std::shared_ptr<KFRole> KFGateModule::FindRole( const std::string& token )
    {
        return _token_list.Find( token );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGateModule::SendLoginAckMessage( uint64 session_id, uint32 result, uint64 ban_time )
    {
        // 消息到这里的都是错误结果
        _kf_display->SendToClient( session_id, result, ban_time );

        // 1秒后主动断开游戏
        _kf_tcp_server->CloseNetHandle( session_id, 1000, __FUNC_LINE__ );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginReq, KFMsg::MsgLoginReq )
    {
        auto session_id = __ROUTE_SERVER_ID__;

        auto& token = kfmsg->token();
        auto account_id = kfmsg->accountid();

        __LOG_DEBUG__( "session[{}] account_id[{}] version[{}] token[{}] login req", session_id, account_id, kfmsg->version(), token );
        // 注册连接器
        if ( !_kf_tcp_server->RegisteNetHandle( session_id, session_id, _invalid_int ) )
        {
            return __LOG_ERROR__( "account_id[{}] register[{}] failed", account_id, session_id );
        }

        // 消息是否有效
        if ( account_id == 0u || token.empty() )
        {
            return SendLoginAckMessage( session_id, KFMsg::LoginTokenError, 0 );
        }

        // 判断版本号
        auto compatibility = KFGlobal::Instance()->CheckVersionCompatibility( kfmsg->version() );
        if ( !compatibility )
        {
            return SendLoginAckMessage( session_id, KFMsg::VersionNotCompatibility, 0 );
        }

        // 服务器正在关闭中
        if ( _is_server_shutdown )
        {
            return SendLoginAckMessage( session_id, KFMsg::LoginIsClose, 0 );
        }

        auto role = FindRole( token );
        if ( role != nullptr )
        {
            // 判断token
            if ( role->_token != token || role->_account_id != account_id )
            {
                return SendLoginAckMessage( session_id, KFMsg::LoginTokenError, 0 );
            }

            // 设置连接成功
            __UN_TIMER_1__( role->_id );
            role->_session_id = session_id;

            // 发送重新登录消息
            KFMsg::S2SReLoginToGameReq req;
            req.set_token( token );
            req.set_playerid( role->_id );
            req.set_sessionid( session_id );
            req.set_accountid( account_id );
            auto ok = role->SendToGame( KFMsg::S2S_RELOGIN_TO_GAME_REQ, &req );
            if ( !ok )
            {
                __LOG_ERROR__( "session[{}] account_id[{}] relogin failed", session_id, account_id );

                // 发送错误
                SendLoginAckMessage( session_id, KFMsg::LoginSystemBusy, 0 );
            }
        }
        else
        {
            LoginToLogin( session_id, account_id, token );
        }
    }

    void KFGateModule::LoginToLogin( uint64 session_id, uint64 account_id, const std::string& token )
    {
        // 没有可用的login
        if ( _login_server_id == _invalid_int )
        {
            return SendLoginAckMessage( session_id, KFMsg::LoginNoLoginServer, 0 );
        }

        // ip
        auto& ip = _kf_tcp_server->GetHandleIp( session_id );

        // 发送到Login服务器验证
        KFMsg::S2SLoginToLoginReq req;
        req.set_ip( ip );
        req.set_token( token );
        req.set_accountid( account_id );
        req.set_sessionid( session_id );
        auto ok = _kf_tcp_client->SendNetMessage( _login_server_id, KFMsg::S2S_LOGIN_TO_LOGIN_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "session[{}] account_id[{}] send login failed", session_id, account_id );

            // 发送错误
            SendLoginAckMessage( session_id, KFMsg::LoginSystemBusy, 0 );
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
        auto login_data = &kfmsg->pblogin();
        __LOG_DEBUG__( "player[{}:{}] session[{}] enter game", login_data->accountid(), login_data->playerid(), login_data->sessionid() );

        auto role = FindRole( login_data->playerid() );
        if ( role != nullptr )
        {
            // 如果已经掉线
            if ( role->_session_id == _invalid_int )
            {
                return __LOG_DEBUG__( "player=[{}] disconnect", login_data->playerid() );
            }

            // 已经在线, 应该是Game宕机后重新加载的逻辑
            role->_game_id = __ROUTE_SERVER_ID__;
        }
        else
        {
            // 绑定角色id
            if ( !_kf_tcp_server->BindObjectId( login_data->sessionid(), login_data->playerid() ) )
            {
                // 发送消息给game, 踢掉玩家
                KFMsg::S2SLeaveToGameReq req;
                req.set_playerid( login_data->playerid() );
                _kf_tcp_client->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_LEAVE_TO_GAME_REQ, &req );
                return __LOG_ERROR__( "player[{}:{}] session[{}] failed", login_data->accountid(), login_data->playerid(), login_data->sessionid() );
            }

            // 删除掉线定时器
            __UN_TIMER_1__( login_data->playerid() );

            // 创建角色
            role = __MAKE_SHARED__( KFRole );
            role->_id = login_data->playerid();
            role->_token = login_data->token();
            role->_game_id = __ROUTE_SERVER_ID__;
            role->_session_id = login_data->sessionid();
            role->_account_id = login_data->accountid();
            AddRole( role );
        }

        // 通知进入游戏
        KFMsg::MsgLoginAck ack;
        ack.set_playerid( login_data->playerid() );
        ack.set_servertime( kfmsg->servertime() );
        ack.mutable_playerdata()->CopyFrom( kfmsg->playerdata() );
        auto ok = role->SendToClient( KFMsg::MSG_LOGIN_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] session[{}] enter failed", login_data->accountid(), login_data->playerid(), login_data->sessionid() );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFGateModule::OnPlayerDisconnection )
    {
        auto role = FindRole( net_data->_id );
        if ( role == nullptr )
        {
            return;
        }

        __LOG_DEBUG__( "client[{}] disconnection", net_data->_id );

        KFMsg::S2SDisconnectToGameReq req;
        req.set_playerid( role->_id );
        role->SendToGame( KFMsg::S2S_DISCONNECT_TO_GAME_REQ, &req );

        // 添加一个定时器
        role->_session_id = _invalid_int;

        static auto _disconnect_time_constant = KFGlobal::Instance()->FindConstant( __STRING__( disconnecttime ) );
        __COUNT_TIMER_1__( role->_id, _disconnect_time_constant->_uint32_value, 1, &KFGateModule::OnTimerPlayerDisconnetion );
    }

    __KF_TIMER_FUNCTION__( KFGateModule::OnTimerPlayerDisconnetion )
    {
        auto role = FindRole( object_id );
        if ( role == nullptr )
        {
            return;
        }

        KFMsg::S2SLeaveToGameReq req;
        req.set_playerid( object_id );
        role->SendToGame( KFMsg::S2S_LEAVE_TO_GAME_REQ, &req );

        // 删除玩家
        RemoveRole( role );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleKickPlayerToGateReq, KFMsg::S2SKickPlayerToGateReq )
    {
        auto role = FindRole( kfmsg->playerid() );
        if ( role == nullptr )
        {
            return;
        }

        // 通知客户端你被踢了
        KFMsg::MsgTellBeKick kick;
        kick.set_type( kfmsg->type() );
        role->SendToClient( KFMsg::MSG_TELL_BE_KICK, &kick );

        // 删除玩家
        RemoveRole( role );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLogoutReq, KFMsg::MsgLogoutReq )
    {
        auto player_id = __ROUTE_SEND_ID__;
        auto role = FindRole( player_id );
        if ( role != nullptr )
        {
            __LOG_DEBUG__( "player[{}] login out", player_id );

            KFMsg::S2SLogoutToGameReq req;
            req.set_playerid( player_id );
            role->SendToGame( KFMsg::S2S_LOGOUT_TO_GAME_REQ, &req );

            // 删除玩家
            RemoveRole( role );
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
