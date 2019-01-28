#include "KFGameModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGameModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFGameModule::OnClientLostServer );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFGameModule::OnClientConnectionServer );

        __REGISTER_ROUTE_MESSAGE_FUNCTION__( &KFGameModule::TransmitMessageToPlayer );
        __REGISTER_CLIENT_TRANSMIT_FUNCTION__( &KFGameModule::TransmitMessageToPlayer );

        _kf_player->BindAfterLoadFunction( this, &KFGameModule::OnAfterLoadPlayerData );
        _kf_player->RegisterEnterFunction( this, &KFGameModule::OnEnterGame );
        _kf_player->RegisterLeaveFunction( this, &KFGameModule::OnLeaveGame );

        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( shutdown ), &KFGameModule::OnDeployShutDownServer );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_GAME_REQ, &KFGameModule::HandleLoginToGameReq );


        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME, &KFGameModule::HandleBroadcastMessageReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_GAME_PLAYER_REQ, &KFGameModule::HandleKickGamePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_DISCONNECTION_REQ, &KFGameModule::HandlePlayerDisconnectionReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ, &KFGameModule::HandleLoginOutReq );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFGameModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_ROUTE_MESSAGE_FUNCTION__();
        __UNREGISTER_CLIENT_TRANSMIT_FUNCTION__();

        _kf_player->UnBindAfterLoadFunction( this );
        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );

        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( shutdown ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_GAME_REQ );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_GAME_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_DISCONNECTION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFGameModule::OnClientConnectionServer )
    {
        if ( servertype != __KF_STRING__( world ) )
        {
            return;
        }

        _world_server_id = serverid;

        // 同步当前在线玩家到World服务器
        KFMsg::S2SGameSyncOnlineReq req;
        auto player = _kf_player->FirstPlayer();
        while ( player != nullptr )
        {
            req.add_playerid( player->GetKeyID() );
            player = _kf_player->NextPlayer();
        }
        SendToWorld( KFMsg::S2S_GAME_SYNC_ONLINE_REQ, &req );
    }

    __KF_CLIENT_LOST_FUNCTION__( KFGameModule::OnClientLostServer )
    {
        if ( servertype == __KF_STRING__( world ) )
        {
            _world_server_id = _invalid_int;
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
        if ( serverid == KFGlobal::Instance()->_app_id._union._id )
        {
            auto player = _kf_player->FindPlayer( playerid );
            if ( player == nullptr )
            {
                return false;
            }

            return SendToClient( player, msgid, message );
        }

        // 本小区
        KFAppID appid( serverid );
        if ( appid._union._app_data._zone_id == KFGlobal::Instance()->_app_id._union._app_data._zone_id )
        {
            return TransmitToPlayer( playerid, msgid, message );
        }

        // 转发到别的小区
        return _kf_route->SendToPlayer( sendid, serverid, playerid, msgid, message );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFGameModule::BroadcastToGate( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToGate req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_TO_GATE, &req );
        return true;
    }

    // 转发服务器
    bool KFGameModule::BroadcastToGame( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToGame req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendToWorld( KFMsg::S2S_BROADCAST_TO_GAME, &req );
    }

    bool KFGameModule::TransmitToPlayer( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2STransmitToPlayer req;
        req.set_playerid( playerid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendToWorld( KFMsg::S2S_TRANSMIT_TO_PLAYER, &req );
    }

    __KF_TRANSMIT_MESSAGE_FUNCTION__( KFGameModule::TransmitMessageToPlayer )
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_COMMAND_FUNCTION__( KFGameModule::OnDeployShutDownServer )
    {
        auto player = _kf_player->FirstPlayer();
        while ( player != nullptr )
        {
            _kf_player->SavePlayer( player );
            player = _kf_player->NextPlayer();
        }
    }

    void KFGameModule::OnEnterGame( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto playerid = player->GetKeyID();
        {
            // 发送消息到世界服务器
            KFMsg::S2SPlayerEnterWorldReq req;
            req.set_playerid( playerid );
            req.set_accountid( kfobject->GetValue( __KF_STRING__( accountid ) ) );
            SendToWorld( KFMsg::S2S_PLAYER_ENTER_WORLD_REQ, &req );
        }

        // 添加对象
        _kf_route->AddObject( playerid );
    }

    void KFGameModule::OnLeaveGame( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto playerid = player->GetKeyID();
        {
            // 发送消息到世界服务器
            KFMsg::S2SPlayerLeaveWorldReq req;
            req.set_playerid( playerid );
            req.set_accountid( kfobject->GetValue( __KF_STRING__( accountid ) ) );
            SendToWorld( KFMsg::S2S_PLAYER_LEAVE_WORLD_REQ, &req );
        }

        // 删除对象
        _kf_route->RemoveObject( playerid );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLoginToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginToGameReq );

        auto pblogin = &kfmsg.pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] login game req!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        // 踢掉在线玩家
        KickPlayer( pblogin->playerid(), KFMsg::KickEnum::KickByLogin, __FUNC_LINE__ );

        // 加载玩家数据
        auto ok = _kf_player->LoadPlayer( pblogin );
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

        KFMsg::S2SLoginToGateAck ack;
        ack.set_result( result );
        ack.mutable_pblogin()->CopyFrom( *pblogin );
        ack.mutable_playerdata()->CopyFrom( *pbplayerdata );
        ack.set_servertime( KFGlobal::Instance()->_real_time );
        auto ok = SendToGate( pblogin->gateid(), KFMsg::S2S_LOGIN_GAME_ACK, &ack );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}:{}] load game ok!", pblogin->accountid(), pblogin->playerid() );
        }
        else
        {
            __LOG_ERROR__( "player[{}:{}] load game[{}] failed!", pblogin->accountid(), pblogin->playerid(), result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandlePlayerDisconnectionReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerDisconnectionReq );

        __LOG_DEBUG__( "player[{}] disconnection!", kfmsg.playerid() );

        _kf_player->RemovePlayer( kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleLoginOutReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SLoginOutReq );

        __LOG_DEBUG__( "player[{}] logout!", kfmsg.playerid() );

        // 删除玩家
        _kf_player->RemovePlayer( kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleKickGamePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickGamePlayerReq );

        auto playerid = kfmsg.playerid();
        KickPlayer( playerid, kfmsg.type(), __FUNC_LINE__ );
    }

    void KFGameModule::KickPlayer( uint64 playerid, uint32 type, const char* function, uint32 line )
    {
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        __LOG_DEBUG_FUNCTION__( function, line, "kick player[{}] by type[{}]!", playerid, type );

        // 发送消息到proxy
        KFMsg::S2SKickGatePlayerReq req;
        req.set_playerid( playerid );
        req.set_type( type );
        SendToClient( player, KFMsg::S2S_KICK_GATE_PLAYER_REQ, &req );

        // 删除玩家
        _kf_player->RemovePlayer( playerid );
    }
}
