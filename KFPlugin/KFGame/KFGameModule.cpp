#include "KFGameModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGameModule::BeforeRun()
    {
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFGameModule::OnServerDiscoverGate );
        __REGISTER_SERVER_LOST_FUNCTION__( &KFGameModule::OnServerLostGate );
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFGameModule::OnClientLostWorld );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFGameModule::OnClientConnectionWorld );

        __REGISTER_ROUTE_MESSAGE_FUNCTION__( &KFGameModule::TranspondToPlayer );
        __REGISTER_CLIENT_TRANSPOND_FUNCTION__( &KFGameModule::TranspondToPlayer );

        auto savetime = _kf_option->GetUInt32( __KF_STRING__( playersavetime ) );

        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterEntitySaveFunction( this, &KFGameModule::SavePlayer );
        _kf_component->SetEntityDataMask( KFDataDefine::Data_Save | KFDataDefine::Data_Delete_Save, ( savetime != 0u ? savetime : 60000 ) );

        _kf_player->RegisterEnterFunction( this, &KFGameModule::OnEnterGame );
        _kf_player->RegisterLeaveFunction( this, &KFGameModule::OnLeaveGame );
        _kf_data_client->BindLoadPlayerFunction( this, &KFGameModule::OnAfterLoadPlayerData );

        __REGISTER_DEPLOY_FUNCTION__( __KF_STRING__( shutdown ), &KFGameModule::OnDeployShutDownServer );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_GAME_REQ, &KFGameModule::HandleLoginToGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_GAME_REQ, &KFGameModule::HandleKickPlayerToGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_RELOGIN_TO_GAME_REQ, &KFGameModule::HandleReLoginToGameReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_CONNECT_TO_GAME_REQ, &KFGameModule::HandleConnectToGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DISCONNECT_TO_GAME_REQ, &KFGameModule::HandleDisconnectToGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LEAVE_TO_GAME_REQ, &KFGameModule::HandleLeaveToGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGOUT_TO_GAME_REQ, &KFGameModule::HandleLogoutToGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME_ACK, &KFGameModule::HandleBroadcastToGameAck );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFGameModule::BeforeShut()
    {
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_ROUTE_MESSAGE_FUNCTION__();
        __UNREGISTER_CLIENT_TRANSPOND_FUNCTION__();

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        _kf_data_client->UnBindLoadPlayerFunction( this );
        _kf_component->UnRegisterEntitySaveFunction();

        __UNREGISTER_DEPLOY_FUNCTION__( __KF_STRING__( shutdown ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CONNECT_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DISCONNECT_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_RELOGIN_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LEAVE_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGOUT_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME_ACK );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_DEPLOY_FUNCTION__( KFGameModule::OnDeployShutDownServer )
    {
        auto player = _kf_player->FirstPlayer();
        while ( player != nullptr )
        {
            SavePlayer( player );
            player = _kf_player->NextPlayer();
        }
    }

    void KFGameModule::SavePlayer( KFEntity* player )
    {
        if ( !player->IsInited() || !player->IsNeedToSave() )
        {
            return;
        }

        player->SetNeetToSave( false );

        // 保存数据库
        static KFMsg::PBObject pbplayerdata;
        _kf_kernel->SerializeToData( player->GetData(), &pbplayerdata );
        _kf_data_client->SavePlayerData( player->GetKeyID(), &pbplayerdata );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFGameModule::OnServerDiscoverGate )
    {
        if ( netdata->_type != __KF_STRING__( gate ) )
        {
            return;
        }

        KFMsg::S2SAddGateToWorldReq req;
        req.set_gameid( KFGlobal::Instance()->_app_id->GetId() );
        req.add_gateid( netdata->_id );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( world ), KFMsg::S2S_ADD_GATE_TO_WORLD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFGameModule::OnServerLostGate )
    {
        if ( netdata->_type != __KF_STRING__( gate ) )
        {
            return;
        }

        KFMsg::S2SRemoveGateToWorldReq req;
        req.set_gameid( KFGlobal::Instance()->_app_id->GetId() );
        req.set_gateid( netdata->_id );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( world ), KFMsg::S2S_REMOVE_GATE_TO_WORLD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFGameModule::OnClientConnectionWorld )
    {
        if ( netdata->_type == __KF_STRING__( world ) )
        {
            _world_hash.AddHashNode( netdata->_type, netdata->_id, 100 );
            _world_server_id = _world_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );

            // 把gate信息发送给world
            {
                NetDataList outlist;
                _kf_tcp_server->GetHandleList( outlist );

                KFMsg::S2SAddGateToWorldReq req;
                req.set_gameid( KFGlobal::Instance()->_app_id->GetId() );
                for ( auto ipaddress : outlist )
                {
                    if ( netdata->_type == __KF_STRING__( gate ) )
                    {
                        req.add_gateid( netdata->_id );
                    }
                }
                _kf_tcp_client->SendNetMessage( netdata->_id, KFMsg::S2S_ADD_GATE_TO_WORLD_REQ, &req );
            }
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFGameModule::OnClientLostWorld )
    {
        if ( netdata->_type == __KF_STRING__( world ) )
        {
            _world_hash.RemoveHashNode( netdata->_id );
            if ( _world_server_id == netdata->_id )
            {
                _world_server_id = _world_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFGameModule::SendToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( _world_server_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _world_server_id, msgid, message );
    }

    void KFGameModule::SendToGate( uint32 msgid, ::google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msgid, message );
    }

    // 发送消息到Gate服务器
    bool KFGameModule::SendToGate( uint64 gateid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, msgid, message );
    }

    // 发送消息到客户端
    bool KFGameModule::SendToClient( uint64 gateid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, playerid, msgid, message );
    }

    // 发送消息到客户端
    bool KFGameModule::SendToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kfobject = player->GetData();
        auto gateid = kfobject->GetValue( __KF_STRING__( gateid ) );
        return _kf_tcp_server->SendNetMessage( gateid, player->GetKeyID(), msgid, message );
    }

    // 发送到玩家
    bool KFGameModule::SendToPlayer( uint64 sendid, KFData* kfbasic, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto serverid = kfbasic->GetValue( __KF_STRING__( serverid ) );
        auto playerid = kfbasic->GetValue( __KF_STRING__( id ) );
        if ( serverid == _invalid_int || playerid == _invalid_int )
        {
            return false;
        }

        return SendToPlayer( sendid, serverid, playerid, msgid, message );
    }

    // 发送到玩家
    bool KFGameModule::SendToPlayer( uint64 sendid, uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        // 本服务器
        if ( serverid == KFGlobal::Instance()->_app_id->GetId() )
        {
            auto player = _kf_player->FindPlayer( playerid );
            if ( player == nullptr )
            {
                return false;
            }

            return SendToClient( player, msgid, message );
        }

        // 转发消息
        return _kf_route->SendToPlayer( sendid, serverid, playerid, msgid, message );
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

        auto gateid = player->GetData()->GetValue( __KF_STRING__( gateid ) );
        return _kf_tcp_server->SendNetMessage( gateid, playerid, msgid, data, length );
    }

    void KFGameModule::BroadcastToGate( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToGateReq req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_TO_GATE_REQ, &req );
    }

    // 转发服务器
    void KFGameModule::BroadcastToGame( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToGameReq req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        SendToWorld( KFMsg::S2S_BROADCAST_TO_GAME_REQ, &req );
    }

    // 转发服务器
    void KFGameModule::BroadcastToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToWorldReq req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        SendToWorld( KFMsg::S2S_BROADCAST_TO_WORLD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleBroadcastToGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastToGameAck );

        KFMsg::S2SBroadcastToGateReq req;
        req.set_msgid( kfmsg.msgid() );
        req.set_msgdata( kfmsg.msgdata() );
        req.set_serial( kfmsg.serial() );
        req.set_worldid( kfmsg.worldid() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_TO_GATE_REQ, &req );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGameModule::OnEnterGame( KFEntity* player )
    {
        auto playerid = player->GetKeyID();
        {
            // 发送消息到世界服务器
            KFMsg::S2SPlayerEnterToWorldReq req;
            req.set_playerid( playerid );
            SendToWorld( KFMsg::S2S_PLAYER_ENTER_TO_WORLD_REQ, &req );
        }
    }

    void KFGameModule::OnLeaveGame( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto playerid = player->GetKeyID();
        {
            // 发送消息到世界服务器
            KFMsg::S2SPlayerLeaveToWorldReq req;
            req.set_playerid( playerid );
            SendToWorld( KFMsg::S2S_PLAYER_LEAVE_TO_WORLD_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLoginToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginToGameReq );

        auto pblogin = &kfmsg.pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] login game req!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        // 踢掉在线玩家
        KickPlayer( pblogin->playerid(), KFMsg::KickEnum::KickByLogin, __FUNC_LINE__ );

        // 加载玩家数据
        auto ok = _kf_data_client->LoadPlayerData( pblogin );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}:{}:{}] login game ok!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );
        }
        else
        {
            __LOG_ERROR__( "player[{}:{}:{}] login game failed!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );
        }
    }

    void KFGameModule::OnAfterLoadPlayerData( uint32 result, const KFMsg::PBLoginData* pblogin, KFMsg::PBObject* pbplayerdata )
    {
        __LOG_DEBUG__( "player[{}:{}:{}] load data[{}] ack!", pblogin->account(), pblogin->accountid(), pblogin->playerid(), result );

        if ( result != KFMsg::Ok )
        {
            KFMsg::S2SLoginToGateAck ack;
            ack.set_result( result );
            ack.set_accountid( pblogin->accountid() );
            ack.set_sessionid( pblogin->sessionid() );
            auto ok = SendToGate( pblogin->gateid(), KFMsg::S2S_LOGIN_TO_GATE_ACK, &ack );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}:{}] load [{}] failed!", pblogin->accountid(), pblogin->playerid(), result );
            }
        }
        else
        {
            // 创建玩家
            auto player = _kf_player->CreatePlayer( pblogin, pbplayerdata );

            // 序列化玩家数据
            _kf_kernel->SerializeToClient( player->GetData(), pbplayerdata );

            KFMsg::S2SEnterToGateAck ack;
            ack.mutable_pblogin()->CopyFrom( *pblogin );
            ack.mutable_playerdata()->CopyFrom( *pbplayerdata );
            ack.set_servertime( KFGlobal::Instance()->_real_time );
            auto ok = SendToGate( pblogin->gateid(), KFMsg::S2S_ENTER_TO_GATE_ACK, &ack );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}:{}] send failed!", pblogin->accountid(), pblogin->playerid() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleReLoginToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SReLoginToGameReq );

        auto player = _kf_player->FindPlayer( kfmsg.playerid() );
        if ( player != nullptr )
        {
            auto kfobject = player->GetData();
            kfobject->SetValue( __KF_STRING__( gateid ), __ROUTE_RECV_ID__ );

            KFMsg::S2SEnterToGateAck ack;
            ack.set_servertime( KFGlobal::Instance()->_real_time );

            auto pblogin = ack.mutable_pblogin();
            pblogin->set_token( kfmsg.token() );
            pblogin->set_playerid( kfmsg.playerid() );
            pblogin->set_sessionid( kfmsg.sessionid() );
            pblogin->set_accountid( kfmsg.accountid() );

            _kf_kernel->SerializeToClient( kfobject, ack.mutable_playerdata() );
            auto ok = SendToGate( __ROUTE_SERVER_ID__, KFMsg::S2S_ENTER_TO_GATE_ACK, &ack );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}:{}] relogin failed!", kfmsg.accountid(), kfmsg.playerid() );
            }
        }
        else
        {
            KFMsg::S2SReLoginToGateAck ack;
            ack.set_token( kfmsg.token() );
            ack.set_playerid( kfmsg.playerid() );
            ack.set_sessionid( kfmsg.sessionid() );
            ack.set_accountid( kfmsg.accountid() );
            auto ok = SendToGate( __ROUTE_SERVER_ID__, KFMsg::S2S_RELOGIN_TO_GATE_ACK, &ack );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}:{}] ack failed!", kfmsg.accountid(), kfmsg.playerid() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleConnectToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SConnectToGameReq );

        for ( auto i = 0; i < kfmsg.pblogin_size(); ++i )
        {
            auto pblogin = &kfmsg.pblogin( i );
            auto player = _kf_player->FindPlayer( pblogin->playerid() );
            if ( player == nullptr )
            {
                _kf_data_client->LoadPlayerData( pblogin );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleDisconnectToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDisconnectToGameReq );

        __LOG_DEBUG__( "player[{}] disconnect!", kfmsg.playerid() );
        auto player = _kf_player->FindPlayer( kfmsg.playerid() );
        if ( player == nullptr )
        {
            return;
        }

        // 掉线了, 先保存玩家信息
        // 后面会加上各种逻辑回调
        SavePlayer( player );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLeaveToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLeaveToGameReq );

        __LOG_DEBUG__( "player[{}] lost!", kfmsg.playerid() );

        _kf_player->RemovePlayer( kfmsg.playerid() );
        _kf_data_client->RemoveLoadData( kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLogoutToGameReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SLogoutToGameReq );

        __LOG_DEBUG__( "player[{}] logout!", kfmsg.playerid() );

        // 删除玩家
        _kf_player->RemovePlayer( kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleKickPlayerToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickPlayerToGameReq );

        auto ok = KickPlayer( kfmsg.playerid(), kfmsg.type(), __FUNC_LINE__ );
        if ( !ok )
        {
            KFMsg::S2SKickPlayerToWorldAck ack;
            ack.set_playerid( kfmsg.playerid() );
            SendToWorld( KFMsg::S2S_KICK_PLAYER_TO_WORLD_ACK, &ack );
        }
    }

    bool KFGameModule::KickPlayer( uint64 playerid, uint32 type, const char* function, uint32 line )
    {
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return false;
        }

        __LOG_DEBUG_FUNCTION__( function, line, "kick player[{}] by type[{}]!", playerid, type );

        // 发送消息到gate服务器
        KFMsg::S2SKickPlayerToGateReq req;
        req.set_type( type );
        req.set_playerid( playerid );
        SendToClient( player, KFMsg::S2S_KICK_PLAYER_TO_GATE_REQ, &req );

        // 删除玩家
        _kf_player->RemovePlayer( playerid );
        return true;
    }
}
