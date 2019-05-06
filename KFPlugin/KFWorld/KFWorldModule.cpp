#include "KFWorldModule.hpp"

namespace KFrame
{
#define __LOGIN_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( login ) )

    void KFWorldModule::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( 0, 5000, 1000, &KFWorldModule::OnTimerWorldRegister );
        __REGISTER_SERVER_LOST_FUNCTION__( &KFWorldModule::OnServerLostGame );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_GATE_TO_WORLD_REQ, &KFWorldModule::HandleAddGateToWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_REMOVE_GATE_TO_WORLD_REQ, &KFWorldModule::HandleRemoveGateToWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_WORLD_REQ, &KFWorldModule::HandleLoginToWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_TO_WORLD_REQ, &KFWorldModule::HandlePlayerEnterToWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_TO_WORLD_REQ, &KFWorldModule::HandlePlayerLeaveToWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_WORLD_REQ, &KFWorldModule::HandleKickPlayerToWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_WORLD_ACK, &KFWorldModule::HandleKickPlayerToWorldAck );

        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME_REQ, &KFWorldModule::HandleBroadcastToGameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_WORLD_REQ, &KFWorldModule::HandleBroadcastToWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_WORLD_ACK, &KFWorldModule::HandleBroadcastToWorldAck );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( kickonline ), true, &KFWorldModule::HandleHttpKickOnline );
    }

    void KFWorldModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_GATE_TO_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REMOVE_GATE_TO_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_TO_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_TO_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_WORLD_ACK );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_WORLD_ACK );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( kickonline ) );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFWorldModule::OnTimerWorldRegister )
    {
        // 更新给auth
        static auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( worldregister );

        __JSON_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( world ), KFGlobal::Instance()->_app_id->ToString() );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( url ), _kf_http_server->GetHttpUrl() );
        auto resultdata = _kf_http_client->STGet( _url, sendjson );

        __JSON_PARSE_STRING__( recvjson, resultdata );
        auto retcode = __JSON_GET_UINT32__( recvjson, __KF_STRING__( retcode ) );
        if ( retcode == KFMsg::Ok )
        {
            __UNREGISTER_OBJECT_TIMER__( objectid );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleAddGateToWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddGateToWorldReq );

        for ( auto i = 0; i < kfmsg.gateid_size(); ++i )
        {
            auto gateid = kfmsg.gateid( i );
            auto kfconhash = _gate_conhash.Create( gateid );
            kfconhash->AddHashNode( __KF_STRING__( game ), kfmsg.gameid(), 50 );

            __LOG_INFO__( "add gate=[{}] game=[{}]!", KFAppId::ToString( gateid ), KFAppId::ToString( kfmsg.gameid() ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleRemoveGateToWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRemoveGateToWorldReq );

        auto kfconhash = _gate_conhash.Find( kfmsg.gateid() );
        if ( kfconhash == nullptr )
        {
            return;
        }

        kfconhash->RemoveHashNode( kfmsg.gameid() );

        __LOG_ERROR__( "remove gate=[{}] game=[{}]!", KFAppId::ToString( kfmsg.gateid() ), KFAppId::ToString( kfmsg.gameid() ) );
    }

    __KF_NET_EVENT_FUNCTION__( KFWorldModule::OnServerLostGame )
    {
        if ( netdata->_type != __KF_STRING__( game ) )
        {
            return;
        }

        // 删除节点
        for ( auto& iter : _gate_conhash._objects )
        {
            auto kfconhash = iter.second;
            auto ok = kfconhash->RemoveHashNode( netdata->_id );
            if ( ok )
            {
                __LOG_ERROR__( "remove gate=[{}] game=[{}]!", KFAppId::ToString( iter.first ), KFAppId::ToString( netdata->_id ) );
            }
        }
    }

    uint64 KFWorldModule::FindLoginGame( uint64 gateid, uint64 playerid )
    {
        auto kfconhash = _gate_conhash.Find( gateid );
        if ( kfconhash == nullptr )
        {
            return _invalid_int;
        }

        return kfconhash->FindHashNode( playerid );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleLoginToWorldReq )
    {
        auto loginid = __ROUTE_SERVER_ID__;
        __PROTO_PARSE__( KFMsg::S2SLoginToWorldReq );

        auto pblogin = kfmsg.mutable_pblogin();
        __LOG_DEBUG__( "player[{}:{}] login world req!", pblogin->account(), pblogin->accountid() );

        // 查询玩家id
        auto playerid = QueryCreatePlayerId( pblogin->accountid() );
        if ( playerid == _invalid_int )
        {
            return SendLoginAckToLogin( KFMsg::LoginDatabaseBusy, loginid, pblogin->gateid(), pblogin->accountid(), pblogin->sessionid() );
        }

        pblogin->set_playerid( playerid );

        // 踢掉已经在线的玩家, 只有踢下线以后才能登陆
        {
            auto ok = KickOnline( KFMsg::KickByLogin, pblogin->playerid(), __FUNC_LINE__ );
            if ( ok )
            {
                return SendLoginAckToLogin( KFMsg::LoginAlreadyOnline, loginid, pblogin->gateid(), pblogin->accountid(), pblogin->sessionid() );
            }
        }

        // 选择Game服务器
        auto gameid = FindLoginGame( pblogin->gateid(), pblogin->playerid() );
        if ( gameid == _invalid_int )
        {
            return SendLoginAckToLogin( KFMsg::LoginNoGameServer, loginid, pblogin->gateid(), pblogin->accountid(), pblogin->sessionid() );
        }

        // 登录请求到game服务器
        KFMsg::S2SLoginToGameReq req;
        req.mutable_pblogin()->CopyFrom( *pblogin );
        auto ok = _kf_tcp_server->SendNetMessage( gameid, KFMsg::S2S_LOGIN_TO_GAME_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] login game failed!", pblogin->accountid(), pblogin->playerid() );
            SendLoginAckToLogin( KFMsg::LoginGameServerBusy, loginid, pblogin->gateid(), pblogin->accountid(), pblogin->sessionid() );
        }
    }

    void KFWorldModule::SendLoginAckToLogin( uint32 result, uint64 loginid, uint64 gateid, uint64 accountid, uint64 sessionid )
    {
        KFMsg::S2SLoginToLoginAck ack;
        ack.set_result( result );
        ack.set_gateid( gateid );
        ack.set_accountid( accountid );
        ack.set_sessionid( sessionid );
        auto ok = _kf_tcp_server->SendNetMessage( loginid, KFMsg::S2S_LOGIN_TO_LOGIN_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] world verify result[{}] failed!", accountid, result );
        }
    }

    uint64 KFWorldModule::QueryCreatePlayerId( uint64 accountid )
    {
        auto kfzone = _kf_zone->GetZone();
        auto redisdriver = __LOGIN_REDIS_DRIVER__;

        // 查询是否存在
        auto queryplayerid = redisdriver->QueryUInt64( "hget {}:{} {}", __KF_STRING__( user ), accountid, kfzone->_logic_id );
        if ( !queryplayerid->IsOk() )
        {
            return _invalid_int;
        }

        // 存在playerid, 直接返回
        if ( queryplayerid->_value != _invalid_int )
        {
            return queryplayerid->_value;
        }

        // 创建playerid
        auto uint64result = redisdriver->Execute( "incr {}:{}", __KF_STRING__( playeridcreate ), kfzone->_id );
        if ( !uint64result->IsOk() || uint64result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        auto newuserid = uint64result->_value + 100000u;
        auto playerid = KFUtility::CalcPlayerid( newuserid, kfzone->_id );
        if ( playerid == _invalid_int )
        {
            return _invalid_int;
        }

        redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( player ), playerid, __KF_STRING__( accountid ), accountid );
        auto voidresult = redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( user ), accountid, kfzone->_logic_id, playerid );
        if ( !voidresult->IsOk() )
        {
            return _invalid_int;
        }

        {
            // 更新给auth
            static auto _update_url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( zonebalance );
            __JSON_DOCUMENT__( sendjson );
            __JSON_SET_VALUE__( sendjson, __KF_STRING__( count ), newuserid );
            __JSON_SET_VALUE__( sendjson, __KF_STRING__( zoneid ), kfzone->_id );
            _kf_http_client->MTGet< KFWorldModule >( _update_url, sendjson );
        }

        return playerid;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandlePlayerEnterToWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerEnterToWorldReq );

        // 更新到数据库
        UpdateOnlineData( kfmsg.playerid(), __ROUTE_SERVER_ID__ );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandlePlayerLeaveToWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLeaveToWorldReq );

        UpdateOnlineData( kfmsg.playerid(), _invalid_int );
    }

    void KFWorldModule::UpdateOnlineData( uint64 playerid, uint64 gameid )
    {
        auto redisdriver = __LOGIN_REDIS_DRIVER__;

        MapString values;
        values[ __KF_STRING__( game ) ] = __TO_STRING__( gameid );
        values[ __KF_STRING__( world ) ] = __TO_STRING__( KFGlobal::Instance()->_app_id->GetId() );
        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( player ), playerid );
    }

    bool KFWorldModule::KickOnline( uint32 type, uint64 playerid, const char* function, uint32 line )
    {
        auto redisdriver = __LOGIN_REDIS_DRIVER__;

        auto kfresult = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( player ), playerid );
        if ( kfresult->_value.empty() )
        {
            return false;
        }

        auto gameid = KFUtility::ToValue( kfresult->_value[ __KF_STRING__( game ) ] );
        if ( gameid == _invalid_int )
        {
            return false;
        }

        __LOG_INFO_FUNCTION__( function, line, "kick[{}] player[{}]!", type, playerid );
        if ( _kf_tcp_server->HaveHandle( gameid ) )
        {
            // 发送踢出消息
            SendKickMessage( type, playerid, gameid );
        }
        else
        {
            auto worldid = KFUtility::ToValue( kfresult->_value[ __KF_STRING__( world ) ] );

            KFMsg::S2SKickPlayerToWorldReq req;
            req.set_type( type );
            req.set_gameid( gameid );
            req.set_playerid( playerid );
            _kf_tcp_client->SendNetMessage( worldid, KFMsg::S2S_KICK_PLAYER_TO_WORLD_REQ, &req );
        }

        return true;
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleKickPlayerToWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickPlayerToWorldReq );

        __LOG_INFO__( "kick[{}] player[{}]!", kfmsg.type(), kfmsg.playerid() );

        // 发送踢出消息
        SendKickMessage( kfmsg.type(), kfmsg.playerid(), kfmsg.gameid() );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleKickPlayerToWorldAck )
    {
        __PROTO_PARSE__( KFMsg::S2SKickPlayerToWorldAck );

        UpdateOnlineData( kfmsg.playerid(), _invalid_int );
    }

    void KFWorldModule::SendKickMessage( uint32 type, uint64 playerid, uint64 gameid )
    {
        KFMsg::S2SKickPlayerToGameReq req;
        req.set_type( type );
        req.set_playerid( playerid );
        auto ok = _kf_tcp_server->SendNetMessage( gameid, KFMsg::S2S_KICK_PLAYER_TO_GAME_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "kick player=[{}] game=[{}]failed!", playerid, KFAppId::ToString( gameid ) );
            UpdateOnlineData( playerid, _invalid_int );
        }
    }

    __KF_HTTP_FUNCTION__( KFWorldModule::HandleHttpKickOnline )
    {
        __JSON_PARSE_STRING__( request, data );

        auto playerid = __JSON_GET_UINT64__( request, __KF_STRING__( playerid ) );
        KickOnline( KFMsg::KickByPlatform, playerid, __FUNC_LINE__ );

        return _invalid_str;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    void KFWorldModule::BroadCastToGame( uint32 msgid, const std::string& msgdata, uint32 serial, uint64 worldid )
    {
        KFMsg::S2SBroadcastToGameAck ack;
        ack.set_msgid( msgid );
        ack.set_msgdata( msgdata );
        ack.set_serial( serial );
        ack.set_worldid( worldid );
        _kf_tcp_server->SendMessageToType( __KF_STRING__( game ), KFMsg::S2S_BROADCAST_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleBroadcastToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastToGameReq );

        BroadCastToGame( kfmsg.msgid(), kfmsg.msgdata(), ++_broadcast_serial, KFGlobal::Instance()->_app_id->GetId() );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleBroadcastToWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastToWorldReq );

        BroadCastToGame( kfmsg.msgid(), kfmsg.msgdata(), ++_broadcast_serial, KFGlobal::Instance()->_app_id->GetId() );

        // 广播给其他world
        KFMsg::S2SBroadcastToWorldAck ack;
        ack.set_msgid( kfmsg.msgid() );
        ack.set_msgdata( kfmsg.msgdata() );
        ack.set_serial( _broadcast_serial );
        ack.set_worldid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( world ), KFMsg::S2S_BROADCAST_TO_WORLD_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleBroadcastToWorldAck )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastToWorldAck );

        BroadCastToGame( kfmsg.msgid(), kfmsg.msgdata(), kfmsg.serial(), kfmsg.worldid() );
    }
}
