#include "KFGameModule.hpp"

namespace KFrame
{
    void KFGameModule::BeforeRun()
    {
        __REGISTER_TCP_SERVER_DISCOVER__( &KFGameModule::OnServerDiscoverGate );
        __REGISTER_TCP_SERVER_LOST__( &KFGameModule::OnServerLostGate );
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFGameModule::OnClientConnectionWorld );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFGameModule::OnClientLostWorld );

        __REGISTER_ROUTE_MESSAGE_FUNCTION__( &KFGameModule::TranspondToPlayer );
        __REGISTER_TCP_CLIENT_FORWARD__( &KFGameModule::TranspondToPlayer );

        _component = _kf_kernel->FindComponent( __STRING__( player ) );
        _component->RegisterEntitySaveFunction( this, &KFGameModule::SavePlayer );

        __REGISTER_PLAYER_ENTER__( &KFGameModule::OnEnterGame );
        __REGISTER_PLAYER_LEAVE__( &KFGameModule::OnLeaveGame );

        _kf_data_client->BindLoadPlayerFunction( this, &KFGameModule::OnAfterLoadPlayerData );
        _kf_data_client->BindQueryPlayerFunction( this, &KFGameModule::OnAfterQueryPlayerData );

        __REGISTER_DEPLOY_FUNCTION__( __STRING__( shutdown ), &KFGameModule::OnDeployShutDownServer );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LOGIN_TO_GAME_REQ, &KFGameModule::HandleLoginToGameReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_RELOGIN_TO_GAME_REQ, &KFGameModule::HandleReLoginToGameReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CONNECT_TO_GAME_REQ, &KFGameModule::HandleConnectToGameReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DISCONNECT_TO_GAME_REQ, &KFGameModule::HandleDisconnectToGameReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LEAVE_TO_GAME_REQ, &KFGameModule::HandleLeaveToGameReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LOGOUT_TO_GAME_REQ, &KFGameModule::HandleLogoutToGameReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_GAME_ACK, &KFGameModule::HandleBroadcastToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_QUERY_PLAYER_REQ, &KFGameModule::HandleQueryPlayerReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_KICK_PLAYER_TO_GAME_REQ, &KFGameModule::HandleKickPlayerToGameReq );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFGameModule::BeforeShut()
    {
        __UN_TCP_SERVER_DISCOVER__();
        __UN_TCP_SERVER_LOST__();
        __UN_TCP_CLIENT_CONNECTION__();
        __UN_TCP_CLIENT_SHUTDOWN__();
        __UN_ROUTE_MESSAGE_FUNCTION__();
        __UN_TCP_CLIENT_FORWARD__();

        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();

        _kf_data_client->UnBindLoadPlayerFunction( this );
        _kf_data_client->UnBindLoadPlayerFunction( this );

        _component->UnRegisterEntitySaveFunction();
        __UN_DEPLOY_FUNCTION__( __STRING__( shutdown ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_LOGIN_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CONNECT_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DISCONNECT_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::S2S_RELOGIN_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::S2S_LEAVE_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::S2S_LOGOUT_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_DEPLOY_FUNCTION__( KFGameModule::OnDeployShutDownServer )
    {
        __LOG_INFO__( "shutdown save player data start=[{}]", param );
        _kf_player->Logout();
        __LOG_INFO__( "shutdown save player data finish=[{}]", param );
    }

    void KFGameModule::SavePlayer( EntityPtr player, uint32 save_flag )
    {
        // 保存数据库
        auto player_data = _kf_kernel->SerializeToData( player );
        _kf_data_client->SavePlayerData( player->GetKeyID(), player_data, save_flag );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFGameModule::OnServerDiscoverGate )
    {
        if ( net_data->_type != __STRING__( gate ) )
        {
            return;
        }

        KFMsg::S2SAddGateToWorldReq req;
        req.add_gateid( net_data->_id );
        req.set_gameid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_tcp_client->SendMessageToType( __STRING__( world ), KFMsg::S2S_ADD_GATE_TO_WORLD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFGameModule::OnServerLostGate )
    {
        if ( net_data->_type != __STRING__( gate ) )
        {
            return;
        }

        KFMsg::S2SRemoveGateToWorldReq req;
        req.set_gateid( net_data->_id );
        req.set_gameid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_tcp_client->SendMessageToType( __STRING__( world ), KFMsg::S2S_REMOVE_GATE_TO_WORLD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFGameModule::OnClientConnectionWorld )
    {
        if ( net_data->_type == __STRING__( world ) )
        {
            _world_hash.AddHashNode( net_data->_type, net_data->_id, 100 );
            _world_server_id = _world_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );

            // 把gate信息发送给world
            {
                NetDataList out_list;
                _kf_tcp_server->GetHandleList( out_list );

                KFMsg::S2SAddGateToWorldReq req;
                req.set_gameid( KFGlobal::Instance()->_app_id->GetId() );
                for ( auto ipaddress : out_list )
                {
                    if ( ipaddress->_type == __STRING__( gate ) )
                    {
                        req.add_gateid( ipaddress->_id );
                    }
                }
                _kf_tcp_client->SendNetMessage( net_data->_id, KFMsg::S2S_ADD_GATE_TO_WORLD_REQ, &req );
            }
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFGameModule::OnClientLostWorld )
    {
        if ( net_data->_type == __STRING__( world ) )
        {
            _world_hash.RemoveHashNode( net_data->_id );
            if ( _world_server_id == net_data->_id )
            {
                _world_server_id = _world_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFGameModule::SendToWorld( uint32 msg_id, ::google::protobuf::Message* message )
    {
        if ( _world_server_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _world_server_id, msg_id, message );
    }

    void KFGameModule::SendToGate( uint32 msg_id, ::google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msg_id, message );
    }

    // 发送消息到Gate服务器
    bool KFGameModule::SendToGate( uint64 gate_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gate_id, msg_id, message );
    }

    // 发送消息到客户端
    bool KFGameModule::SendToClient( uint64 gate_id, uint64 player_id, uint32 msg_id, ::google::protobuf::Message* message, uint32 delay )
    {
        return _kf_tcp_server->SendNetMessage( gate_id, player_id, msg_id, message, delay );
    }

    // 发送消息到客户端
    bool KFGameModule::SendToClient( EntityPtr player, uint32 msg_id, ::google::protobuf::Message* message, uint32 delay )
    {
        if ( !player->IsInited() )
        {
            return false;
        }

        auto gate_id = player->Get( __STRING__( gateid ) );
        if ( gate_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_server->SendNetMessage( gate_id, player->GetKeyID(), msg_id, message, delay );
    }

    // 发送到玩家
    bool KFGameModule::SendToPlayer( uint64 send_id, DataPtr basic_data, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto player_id = basic_data->Get( __STRING__( id ) );
        auto server_id = basic_data->Get( __STRING__( serverid ) );
        if ( server_id == _invalid_int || player_id == _invalid_int )
        {
            return false;
        }

        return SendToPlayer( send_id, server_id, player_id, msg_id, message );
    }

    // 发送到玩家
    bool KFGameModule::SendToPlayer( uint64 send_id, uint64 server_id, uint64 player_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        // 本服务器
        if ( server_id == KFGlobal::Instance()->_app_id->GetId() )
        {
            auto player = _kf_player->FindPlayer( player_id );
            if ( player == nullptr )
            {
                return false;
            }

            return SendToClient( player, msg_id, message );
        }

        // 转发消息
        return _kf_route->SendToEntity( send_id, server_id, player_id, msg_id, message );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_FORWARD_MESSAGE_FUNCTION__( KFGameModule::TranspondToPlayer )
    {
        auto player_id = __ROUTE_RECV_ID__;
        auto player = _kf_player->FindPlayer( player_id );
        if ( player == nullptr )
        {
            return true;
        }

        auto gate_id = player->Get( __STRING__( gateid ) );
        if ( gate_id == _invalid_int )
        {
            return true;
        }

        return _kf_tcp_server->SendNetMessage( gate_id, player_id, msg_id, data, length );
    }

    void KFGameModule::BroadcastToGate( uint32 msg_id, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToGateReq req;
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_TO_GATE_REQ, &req );
    }

    void KFGameModule::BroadcastToServer( uint32 msg_id, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToServerReq req;
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_TO_SERVER_REQ, &req );
    }

    // 转发服务器
    void KFGameModule::BroadcastToGame( uint32 msg_id, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToGameReq req;
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        SendToWorld( KFMsg::S2S_BROADCAST_TO_GAME_REQ, &req );
    }

    // 转发服务器
    void KFGameModule::BroadcastToWorld( uint32 msg_id, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToWorldReq req;
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        SendToWorld( KFMsg::S2S_BROADCAST_TO_WORLD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleBroadcastToGameAck, KFMsg::S2SBroadcastToGameAck )
    {
        KFMsg::S2SBroadcastToGateReq req;
        req.set_msgid( kfmsg->msgid() );
        req.set_msgdata( kfmsg->msgdata() );
        req.set_serial( kfmsg->serial() );
        req.set_worldid( kfmsg->worldid() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_TO_GATE_REQ, &req );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFGameModule::OnEnterGame )
    {
        // 发送消息到世界服务器
        KFMsg::S2SPlayerEnterToWorldReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_accountid( player->Get<uint64>( __STRING__( accountid ) ) );
        SendToWorld( KFMsg::S2S_PLAYER_ENTER_TO_WORLD_REQ, &req );
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFGameModule::OnLeaveGame )
    {
        // 发送消息到世界服务器
        KFMsg::S2SPlayerLeaveToWorldReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_accountid( player->Get<uint64>( __STRING__( accountid ) ) );
        SendToWorld( KFMsg::S2S_PLAYER_LEAVE_TO_WORLD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLoginToGameReq, KFMsg::S2SLoginToGameReq )
    {
        auto login_data = &kfmsg->pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] login game req", login_data->account(), login_data->accountid(), login_data->playerid() );

        // 踢掉在线玩家
        KickPlayer( login_data->playerid(), KFMsg::KickEnum::KickByLogin, __FUNC_LINE__ );

        // 加载玩家数据
        auto ok = _kf_data_client->LoadPlayerData( login_data );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}:{}:{}] login game ok", login_data->account(), login_data->accountid(), login_data->playerid() );
        }
        else
        {
            __LOG_ERROR__( "player[{}:{}:{}] login game failed", login_data->account(), login_data->accountid(), login_data->playerid() );
        }
    }

    void KFGameModule::OnAfterLoadPlayerData( uint32 result, const KFMsg::PBLoginData* login_data, const KFMsg::PBObject* player_data )
    {
        __LOG_DEBUG__( "player[{}:{}:{}] load data[{}] ack", login_data->account(), login_data->accountid(), login_data->playerid(), result );

        if ( result == KFMsg::Ok )
        {
            auto player = _kf_player->Login( login_data, player_data );
            if ( player != nullptr )
            {
                if ( !login_data->loginbycrash() )
                {
                    // 序列化玩家数据
                    auto new_player_data = _kf_kernel->SerializeToOnline( player );

#ifdef __KF_DEBUG__
                    static auto _print_enter = 0u;
                    if ( _print_enter == 1u )
                    {
                        __LOG_DEBUG__( "player=[{}], data=[{}]", player->GetKeyID(), new_player_data->DebugString() );
                    }
#endif

                    KFMsg::S2SEnterToGateAck ack;
                    ack.mutable_pblogin()->CopyFrom( *login_data );
                    ack.mutable_playerdata()->CopyFrom( *new_player_data );
                    ack.set_servertime( KFGlobal::Instance()->_real_time );
                    auto ok = SendToGate( login_data->gateid(), KFMsg::S2S_ENTER_TO_GATE_ACK, &ack );
                    if ( !ok )
                    {
                        __LOG_ERROR__( "player[{}:{}] send failed", login_data->accountid(), login_data->playerid() );
                    }
                }
            }
            else
            {
                SendLoginToGateAck( KFMsg::LoginCreatePlayerError, login_data );
            }
        }
        else
        {
            SendLoginToGateAck( result, login_data );
        }
    }

    void KFGameModule::SendLoginToGateAck( uint32 result, const KFMsg::PBLoginData* login_data )
    {
        KFMsg::S2SLoginToGateAck ack;
        ack.set_result( result );
        ack.set_accountid( login_data->accountid() );
        ack.set_sessionid( login_data->sessionid() );
        auto ok = SendToGate( login_data->gateid(), KFMsg::S2S_LOGIN_TO_GATE_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] load [{}] failed", login_data->accountid(), login_data->playerid(), result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleReLoginToGameReq, KFMsg::S2SReLoginToGameReq )
    {
        auto player = _kf_player->ReLogin( kfmsg->playerid(), __ROUTE_RECV_ID__ );
        if ( player != nullptr )
        {
            KFMsg::S2SEnterToGateAck ack;
            ack.set_servertime( KFGlobal::Instance()->_real_time );

            auto login_data = ack.mutable_pblogin();
            login_data->set_token( kfmsg->token() );
            login_data->set_playerid( kfmsg->playerid() );
            login_data->set_sessionid( kfmsg->sessionid() );
            login_data->set_accountid( kfmsg->accountid() );

            auto player_data = _kf_kernel->SerializeToOnline( player );
            ack.mutable_playerdata()->CopyFrom( *player_data );
            auto ok = SendToGate( __ROUTE_SERVER_ID__, KFMsg::S2S_ENTER_TO_GATE_ACK, &ack );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}:{}] relogin failed", kfmsg->accountid(), kfmsg->playerid() );
            }
        }
        else
        {
            KFMsg::S2SReLoginToGateAck ack;
            ack.set_token( kfmsg->token() );
            ack.set_playerid( kfmsg->playerid() );
            ack.set_sessionid( kfmsg->sessionid() );
            ack.set_accountid( kfmsg->accountid() );
            auto ok = SendToGate( __ROUTE_SERVER_ID__, KFMsg::S2S_RELOGIN_TO_GATE_ACK, &ack );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}:{}] ack failed", kfmsg->accountid(), kfmsg->playerid() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleConnectToGameReq, KFMsg::S2SConnectToGameReq )
    {
        for ( auto i = 0; i < kfmsg->pblogin_size(); ++i )
        {
            auto login_data = &kfmsg->pblogin( i );
            auto player = _kf_player->FindPlayer( login_data->playerid() );
            if ( player == nullptr )
            {
                _kf_data_client->LoadPlayerData( login_data );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleDisconnectToGameReq, KFMsg::S2SDisconnectToGameReq )
    {
        __FIND_PLAYER_BY_ID__;
        __LOG_INFO__( "player=[{}] disconnect", kfmsg->playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLeaveToGameReq, KFMsg::S2SLeaveToGameReq )
    {
        __LOG_DEBUG__( "player[{}] lost", kfmsg->playerid() );

        _kf_player->Logout( kfmsg->playerid() );
        _kf_data_client->RemoveLoadData( kfmsg->playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLogoutToGameReq, KFMsg::S2SLogoutToGameReq )
    {
        __FIND_PLAYER_BY_ID__;
        __LOG_DEBUG__( "player[{}] logout", kfmsg->playerid() );

        // 设置不在线, 后续不发送消息
        player->Operate( __STRING__( gateid ), KFEnum::Set, _invalid_int );

        // 删除玩家
        _kf_player->Logout( kfmsg->playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleKickPlayerToGameReq, KFMsg::S2SKickPlayerToGameReq )
    {
        KickPlayer( kfmsg->playerid(), kfmsg->type(), __FUNC_LINE__ );
    }

    bool KFGameModule::KickPlayer( uint64 player_id, uint32 type, const char* function, uint32 line )
    {
        auto player = _kf_player->FindPlayer( player_id );
        if ( player == nullptr )
        {
            return false;
        }

        __LOG_DEBUG_FUNCTION__( function, line, "kick player[{}] by type[{}]", player_id, type );

        // 发送消息到gate服务器
        KFMsg::S2SKickPlayerToGateReq req;
        req.set_type( type );
        req.set_playerid( player_id );
        SendToClient( player, KFMsg::S2S_KICK_PLAYER_TO_GATE_REQ, &req );

        // 删除玩家
        player->Operate( __STRING__( gateid ), KFEnum::Set, _invalid_int );
        _kf_player->Logout( player_id );
        return true;
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleQueryPlayerReq, KFMsg::MsgQueryPlayerReq )
    {
        // 不能查询自己的数据，客户端本地可以获取到
        if ( player->GetKeyID() == kfmsg->playerid() )
        {
            return;
        }

        //查询玩家数据
        auto ok = _kf_data_client->QueryPlayerData( player->GetKeyID(), kfmsg->playerid() );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::DataServerBusy );
        }
    }

    void KFGameModule::OnAfterQueryPlayerData( uint32 result, uint64 player_id, const KFMsg::PBObject* player_data )
    {
        auto player = _kf_player->FindPlayer( player_id );
        if ( player == nullptr )
        {
            return;
        }

        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player_id, result );
        }

        KFMsg::MsgQueryPlayerAck ack;
        ack.mutable_player()->CopyFrom( *player_data );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_PLAYER_ACK, &ack );
    }

}
