#include "KFWorldModule.hpp"

namespace KFrame
{
    void KFWorldModule::BeforeRun()
    {
        __LOOP_TIMER_0__( 10000, 5000, &KFWorldModule::OnTimerZoneRegister );
        __REGISTER_TCP_SERVER_LOST__( &KFWorldModule::OnServerLostGame );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ADD_GATE_TO_WORLD_REQ, &KFWorldModule::HandleAddGateToWorldReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_REMOVE_GATE_TO_WORLD_REQ, &KFWorldModule::HandleRemoveGateToWorldReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LOGIN_TO_WORLD_REQ, &KFWorldModule::HandleLoginToWorldReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_PLAYER_ENTER_TO_WORLD_REQ, &KFWorldModule::HandlePlayerEnterToWorldReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_PLAYER_LEAVE_TO_WORLD_REQ, &KFWorldModule::HandlePlayerLeaveToWorldReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_GAME_REQ, &KFWorldModule::HandleBroadcastToGameReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_WORLD_REQ, &KFWorldModule::HandleBroadcastToWorldReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_WORLD_ACK, &KFWorldModule::HandleBroadcastToWorldAck );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP__( __STRING__( kickonline ), true, &KFWorldModule::HandleHttpKickOnline );
    }

    void KFWorldModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_TCP_SERVER_LOST__();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_ADD_GATE_TO_WORLD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_REMOVE_GATE_TO_WORLD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_LOGIN_TO_WORLD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_TO_WORLD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_TO_WORLD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::S2S_BROADCAST_TO_WORLD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_BROADCAST_TO_WORLD_ACK );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_HTTP__( __STRING__( kickonline ) );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFWorldModule::OnTimerZoneRegister )
    {
        auto ok = _kf_dir_client->ZoneRegisterToDir();
        if ( ok )
        {
            __UN_TIMER_0__();
            _kf_dir_client->WorldRegisterToDir( KFGlobal::Instance()->_app_id->GetId(), _kf_http_server->GetHttpUrl() );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleAddGateToWorldReq, KFMsg::S2SAddGateToWorldReq )
    {
        for ( auto i = 0; i < kfmsg->gateid_size(); ++i )
        {
            auto gate_id = kfmsg->gateid( i );
            auto hash_data = _gate_hash.Create( gate_id );
            hash_data->AddHashNode( __STRING__( game ), kfmsg->gameid(), 50 );

            __LOG_INFO__( "add gate=[{}] game=[{}]", KFAppId::ToString( gate_id ), KFAppId::ToString( kfmsg->gameid() ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleRemoveGateToWorldReq, KFMsg::S2SRemoveGateToWorldReq )
    {
        auto hash_data = _gate_hash.Find( kfmsg->gateid() );
        if ( hash_data == nullptr )
        {
            return;
        }

        hash_data->RemoveHashNode( kfmsg->gameid() );

        __LOG_ERROR__( "remove gate=[{}] game=[{}]", KFAppId::ToString( kfmsg->gateid() ), KFAppId::ToString( kfmsg->gameid() ) );
    }

    __KF_NET_EVENT_FUNCTION__( KFWorldModule::OnServerLostGame )
    {
        if ( net_data->_type != __STRING__( game ) )
        {
            return;
        }

        // 删除节点
        for ( auto& iter : _gate_hash._objects )
        {
            auto hash_data = iter.second;
            auto ok = hash_data->RemoveHashNode( net_data->_id );
            if ( ok )
            {
                __LOG_ERROR__( "remove gate=[{}] game=[{}]", KFAppId::ToString( iter.first ), KFAppId::ToString( net_data->_id ) );
            }
        }
    }

    uint64 KFWorldModule::FindLoginGame( uint64 gate_id, uint64 player_id )
    {
        auto hash_data = _gate_hash.Find( gate_id );
        if ( hash_data == nullptr )
        {
            return _invalid_int;
        }

        return hash_data->FindHashNode( player_id );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleLoginToWorldReq, KFMsg::S2SLoginToWorldReq )
    {
        auto login_id = __ROUTE_SERVER_ID__;
        auto login_data = &kfmsg->pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] login world req", login_data->account(), login_data->accountid(), login_data->playerid() );

        // 踢掉已经在线的玩家
        KickOnlineToGame( KFMsg::KickByLogin, login_data->playerid(), _invalid_int, __FUNC_LINE__ );

        // 选择Game服务器
        auto game_id = FindLoginGame( login_data->gateid(), login_data->playerid() );
        if ( game_id == _invalid_int )
        {
            return SendLoginAckToLogin( KFMsg::LoginNoGameServer, login_id, login_data->gateid(), login_data->accountid(), login_data->sessionid() );
        }

        // 登录请求到game服务器
        KFMsg::S2SLoginToGameReq req;
        req.mutable_pblogin()->CopyFrom( *login_data );
        auto ok = _kf_tcp_server->SendNetMessage( game_id, KFMsg::S2S_LOGIN_TO_GAME_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] login game failed", login_data->accountid(), login_data->playerid() );
            SendLoginAckToLogin( KFMsg::LoginGameServerBusy, login_id, login_data->gateid(), login_data->accountid(), login_data->sessionid() );
        }
    }

    void KFWorldModule::SendLoginAckToLogin( uint32 result, uint64 login_id, uint64 gate_id, uint64 account_id, uint64 sessionid )
    {
        KFMsg::S2SLoginToLoginAck ack;
        ack.set_result( result );
        ack.set_gateid( gate_id );
        ack.set_accountid( account_id );
        ack.set_sessionid( sessionid );
        auto ok = _kf_tcp_server->SendNetMessage( login_id, KFMsg::S2S_LOGIN_TO_LOGIN_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] world verify result[{}] failed", account_id, result );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandlePlayerEnterToWorldReq, KFMsg::S2SPlayerEnterToWorldReq )
    {
        UpdatePlayerOnline( kfmsg->accountid(), kfmsg->playerid(), KFGlobal::Instance()->_app_id->GetId(), __ROUTE_SERVER_ID__ );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandlePlayerLeaveToWorldReq, KFMsg::S2SPlayerLeaveToWorldReq )
    {
        UpdatePlayerOnline( kfmsg->accountid(), kfmsg->playerid(), _invalid_int, _invalid_int );
    }

    void KFWorldModule::UpdatePlayerOnline( uint64 account_id, uint64 player_id, uint64 world_id, uint64 game_id )
    {
        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( online );

        __JSON_OBJECT_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __STRING__( accountid ), account_id );
        __JSON_SET_VALUE__( sendjson, __STRING__( playerid ), player_id );
        __JSON_SET_VALUE__( sendjson, __STRING__( world ), world_id );
        __JSON_SET_VALUE__( sendjson, __STRING__( game ), game_id );
        _kf_http_client->MTGet<KFWorldModule>( _url, sendjson );
    }

    void KFWorldModule::KickOnlineToGame( uint32 type, uint64 player_id, uint64 game_id, const char* function, uint32 line )
    {
        __LOG_INFO_FUNCTION__( function, line, "kick=[{}] player=[{}]", type, player_id );

        KFMsg::S2SKickPlayerToGameReq req;
        req.set_type( type );
        req.set_playerid( player_id );
        if ( game_id != _invalid_int )
        {
            _kf_tcp_server->SendNetMessage( game_id, KFMsg::S2S_KICK_PLAYER_TO_GAME_REQ, &req );
        }
        else
        {
            _kf_tcp_server->SendMessageToType( __STRING__( game ), KFMsg::S2S_KICK_PLAYER_TO_GAME_REQ, &req );
        }
    }

    __KF_HTTP_FUNCTION__( KFWorldModule::HandleHttpKickOnline )
    {
        __JSON_PARSE_STRING__( request, data );

        auto type = __JSON_GET_UINT32__( request, __STRING__( type ) );
        auto game_id = __JSON_GET_UINT64__( request, __STRING__( game ) );
        auto player_id = __JSON_GET_UINT64__( request, __STRING__( playerid ) );
        KickOnlineToGame( type, player_id, game_id, __FUNC_LINE__ );
        return _invalid_string;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    void KFWorldModule::BroadCastToGame( uint32 msg_id, const std::string& msg_data, uint32 serial, uint64 world_id )
    {
        KFMsg::S2SBroadcastToGameAck ack;
        ack.set_msgid( msg_id );
        ack.set_msgdata( msg_data );
        ack.set_serial( serial );
        ack.set_worldid( world_id );
        _kf_tcp_server->SendMessageToType( __STRING__( game ), KFMsg::S2S_BROADCAST_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleBroadcastToGameReq, KFMsg::S2SBroadcastToGameReq )
    {
        BroadCastToGame( kfmsg->msgid(), kfmsg->msgdata(), ++_broadcast_serial, KFGlobal::Instance()->_app_id->GetId() );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleBroadcastToWorldReq, KFMsg::S2SBroadcastToWorldReq )
    {
        BroadCastToGame( kfmsg->msgid(), kfmsg->msgdata(), ++_broadcast_serial, KFGlobal::Instance()->_app_id->GetId() );

        // 广播给其他world
        KFMsg::S2SBroadcastToWorldAck ack;
        ack.set_msgid( kfmsg->msgid() );
        ack.set_msgdata( kfmsg->msgdata() );
        ack.set_serial( _broadcast_serial );
        ack.set_worldid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_tcp_client->SendMessageToType( __STRING__( world ), KFMsg::S2S_BROADCAST_TO_WORLD_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleBroadcastToWorldAck, KFMsg::S2SBroadcastToWorldAck )
    {
        BroadCastToGame( kfmsg->msgid(), kfmsg->msgdata(), kfmsg->serial(), kfmsg->worldid() );
    }
}
