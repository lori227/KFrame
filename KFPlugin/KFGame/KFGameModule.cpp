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
        __REGISTER_TCP_CLIENT_TRANSPOND__( &KFGameModule::TranspondToPlayer );

        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        _kf_component->RegisterEntitySaveFunction( this, &KFGameModule::SavePlayer );

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
        __UN_TCP_CLIENT_TRANSPOND__();

        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();

        _kf_data_client->UnBindLoadPlayerFunction( this );
        _kf_data_client->UnBindLoadPlayerFunction( this );

        _kf_component->UnRegisterEntitySaveFunction();
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
        _kf_player->RemovePlayer();
        __LOG_INFO__( "shutdown save player data finish=[{}]", param );
    }

    void KFGameModule::SavePlayer( KFEntity* player, uint32 saveflag )
    {
        // 保存数据库
        auto pbplayerdata = _kf_kernel->SerializeToData( player );
        _kf_data_client->SavePlayerData( player->GetKeyID(), pbplayerdata, saveflag );
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
    bool KFGameModule::SendToGate( uint64 gateid, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, msg_id, message );
    }

    // 发送消息到客户端
    bool KFGameModule::SendToClient( uint64 gateid, uint64 playerid, uint32 msg_id, ::google::protobuf::Message* message, uint32 delay )
    {
        return _kf_tcp_server->SendNetMessage( gateid, playerid, msg_id, message, delay );
    }

    // 发送消息到客户端
    bool KFGameModule::SendToClient( KFEntity* player, uint32 msg_id, ::google::protobuf::Message* message, uint32 delay )
    {
        if ( !player->IsInited() )
        {
            return false;
        }

        auto gateid = player->Get( __STRING__( gateid ) );
        if ( gateid == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_server->SendNetMessage( gateid, player->GetKeyID(), msg_id, message, delay );
    }

    // 发送到玩家
    bool KFGameModule::SendToPlayer( uint64 sendid, DataPtr kfbasic, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto playerid = kfbasic->Get( __STRING__( id ) );
        auto server_id = kfbasic->Get( __STRING__( server_id ) );
        if ( server_id == _invalid_int || playerid == _invalid_int )
        {
            return false;
        }

        return SendToPlayer( sendid, server_id, playerid, msg_id, message );
    }

    // 发送到玩家
    bool KFGameModule::SendToPlayer( uint64 sendid, uint64 server_id, uint64 playerid, uint32 msg_id, ::google::protobuf::Message* message )
    {
        // 本服务器
        if ( server_id == KFGlobal::Instance()->_app_id->GetId() )
        {
            auto player = _kf_player->FindPlayer( playerid );
            if ( player == nullptr )
            {
                return false;
            }

            return SendToClient( player, msg_id, message );
        }

        // 转发消息
        return _kf_route->SendToEntity( sendid, server_id, playerid, msg_id, message );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TRANSPOND_MESSAGE_FUNCTION__( KFGameModule::TranspondToPlayer )
    {
        auto playerid = __ROUTE_RECV_ID__;
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return true;
        }

        auto gateid = player->Get( __STRING__( gateid ) );
        if ( gateid == _invalid_int )
        {
            return true;
        }

        return _kf_tcp_server->SendNetMessage( gateid, playerid, msg_id, data, length );
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
        req.set_msgid( kfmsg->msg_id() );
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
        auto pblogin = &kfmsg->pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] login game req", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        // 踢掉在线玩家
        KickPlayer( pblogin->playerid(), KFMsg::KickEnum::KickByLogin, __FUNC_LINE__ );

        // 加载玩家数据
        auto ok = _kf_data_client->LoadPlayerData( pblogin );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}:{}:{}] login game ok", pblogin->account(), pblogin->accountid(), pblogin->playerid() );
        }
        else
        {
            __LOG_ERROR__( "player[{}:{}:{}] login game failed", pblogin->account(), pblogin->accountid(), pblogin->playerid() );
        }
    }

    void KFGameModule::OnAfterLoadPlayerData( uint32 result, const KFMsg::PBLoginData* pblogin, const KFMsg::PBObject* pbplayerdata )
    {
        __LOG_DEBUG__( "player[{}:{}:{}] load data[{}] ack", pblogin->account(), pblogin->accountid(), pblogin->playerid(), result );

        if ( result == KFMsg::Ok )
        {
            auto player = _kf_player->Login( pblogin, pbplayerdata );
            if ( player != nullptr )
            {
                if ( !pblogin->loginbycrash() )
                {
                    // 序列化玩家数据
                    auto pbnewdata = _kf_kernel->SerializeToOnline( player );

#ifdef __KF_DEBUG__
                    static auto _print_enter = 0u;
                    if ( _print_enter == 1u )
                    {
                        __LOG_DEBUG__( "player=[{}], data=[{}]", player->GetKeyID(), pbnewdata->DebugString() );
                    }
#endif

                    KFMsg::S2SEnterToGateAck ack;
                    ack.mutable_pblogin()->CopyFrom( *pblogin );
                    ack.mutable_playerdata()->CopyFrom( *pbnewdata );
                    ack.set_servertime( KFGlobal::Instance()->_real_time );
                    auto ok = SendToGate( pblogin->gateid(), KFMsg::S2S_ENTER_TO_GATE_ACK, &ack );
                    if ( !ok )
                    {
                        __LOG_ERROR__( "player[{}:{}] send failed", pblogin->accountid(), pblogin->playerid() );
                    }
                }
            }
            else
            {
                SendLoginToGateAck( KFMsg::LoginCreatePlayerError, pblogin );
            }
        }
        else
        {
            SendLoginToGateAck( result, pblogin );
        }
    }

    void KFGameModule::SendLoginToGateAck( uint32 result, const KFMsg::PBLoginData* pblogin )
    {
        KFMsg::S2SLoginToGateAck ack;
        ack.set_result( result );
        ack.set_accountid( pblogin->accountid() );
        ack.set_sessionid( pblogin->sessionid() );
        auto ok = SendToGate( pblogin->gateid(), KFMsg::S2S_LOGIN_TO_GATE_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] load [{}] failed", pblogin->accountid(), pblogin->playerid(), result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleReLoginToGameReq, KFMsg::S2SReLoginToGameReq )
    {
        auto player = _kf_player->ReLogin( kfmsg->playerid(), __ROUTE_RECV_ID__ );
        if ( player != nullptr )
        {
            KFMsg::S2SEnterToGateAck ack;
            ack.set_servertime( KFGlobal::Instance()->_real_time );

            auto pblogin = ack.mutable_pblogin();
            pblogin->set_token( kfmsg->token() );
            pblogin->set_playerid( kfmsg->playerid() );
            pblogin->set_sessionid( kfmsg->sessionid() );
            pblogin->set_accountid( kfmsg->accountid() );

            auto playerdata = _kf_kernel->SerializeToOnline( player );
            ack.mutable_playerdata()->CopyFrom( *playerdata );
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
            auto pblogin = &kfmsg->pblogin( i );
            auto player = _kf_player->FindPlayer( pblogin->playerid() );
            if ( player == nullptr )
            {
                _kf_data_client->LoadPlayerData( pblogin );
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

        _kf_player->RemovePlayer( kfmsg->playerid() );
        _kf_data_client->RemoveLoadData( kfmsg->playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLogoutToGameReq, KFMsg::S2SLogoutToGameReq )
    {
        __FIND_PLAYER_BY_ID__;
        __LOG_DEBUG__( "player[{}] logout", kfmsg->playerid() );

        // 设置不在线, 后续不发送消息
        player->Set( __STRING__( gateid ), _invalid_int );

        // 删除玩家
        _kf_player->RemovePlayer( kfmsg->playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleKickPlayerToGameReq, KFMsg::S2SKickPlayerToGameReq )
    {
        KickPlayer( kfmsg->playerid(), kfmsg->type(), __FUNC_LINE__ );
    }

    bool KFGameModule::KickPlayer( uint64 playerid, uint32 type, const char* function, uint32 line )
    {
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return false;
        }

        __LOG_DEBUG_FUNCTION__( function, line, "kick player[{}] by type[{}]", playerid, type );

        // 发送消息到gate服务器
        KFMsg::S2SKickPlayerToGateReq req;
        req.set_type( type );
        req.set_playerid( playerid );
        SendToClient( player, KFMsg::S2S_KICK_PLAYER_TO_GATE_REQ, &req );

        // 删除玩家
        player->Set( __STRING__( gateid ), _invalid_int );
        _kf_player->RemovePlayer( playerid );
        return true;
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleQueryPlayerReq, KFMsg::MsgQueryPlayerReq )
    {
        // 不能查询自己的数据，客户端本地可以获取到
        if ( kfentity->GetKeyID() == kfmsg->playerid() )
        {
            return;
        }

        //查询玩家数据
        auto ok = _kf_data_client->QueryPlayerData( kfentity->GetKeyID(), kfmsg->playerid() );
        if ( !ok )
        {
            _kf_display->SendToClient( kfentity, KFMsg::DataServerBusy );
        }
    }

    void KFGameModule::OnAfterQueryPlayerData( uint32 result, uint64 playerid, const KFMsg::PBObject* pbplayerdata )
    {
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( playerid, result );
        }



        KFMsg::MsgQueryPlayerAck ack;
        ack.mutable_player()->CopyFrom( *pbplayerdata );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_PLAYER_ACK, &ack );
    }

}
