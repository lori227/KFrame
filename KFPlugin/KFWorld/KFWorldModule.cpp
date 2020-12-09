#include "KFWorldModule.hpp"

namespace KFrame
{
    void KFWorldModule::BeforeRun()
    {
        __LOOP_TIMER_0__( 10000, 5000, &KFWorldModule::OnTimerZoneRegister );
        __REGISTER_TCP_SERVER_LOST__( &KFWorldModule::OnServerLostGame );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFWorldModule, KFMessageEnum::Normal, KFMsg::S2S_ADD_GATE_TO_WORLD_REQ, KFMsg::S2SAddGateToWorldReq, HandleAddGateToWorldReq );
        __REGISTER_MESSAGE__( KFWorldModule, KFMessageEnum::Normal, KFMsg::S2S_REMOVE_GATE_TO_WORLD_REQ, KFMsg::S2SRemoveGateToWorldReq, HandleRemoveGateToWorldReq );
        __REGISTER_MESSAGE__( KFWorldModule, KFMessageEnum::Normal, KFMsg::S2S_LOGIN_TO_WORLD_REQ, KFMsg::S2SLoginToWorldReq, HandleLoginToWorldReq );
        __REGISTER_MESSAGE__( KFWorldModule, KFMessageEnum::Normal, KFMsg::S2S_PLAYER_ENTER_TO_WORLD_REQ, KFMsg::S2SPlayerEnterToWorldReq, HandlePlayerEnterToWorldReq );
        __REGISTER_MESSAGE__( KFWorldModule, KFMessageEnum::Normal, KFMsg::S2S_PLAYER_LEAVE_TO_WORLD_REQ, KFMsg::S2SPlayerLeaveToWorldReq, HandlePlayerLeaveToWorldReq );
        __REGISTER_MESSAGE__( KFWorldModule, KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_GAME_REQ, KFMsg::S2SBroadcastToGameReq, HandleBroadcastToGameReq );
        __REGISTER_MESSAGE__( KFWorldModule, KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_WORLD_REQ, KFMsg::S2SBroadcastToWorldReq, HandleBroadcastToWorldReq );
        __REGISTER_MESSAGE__( KFWorldModule, KFMessageEnum::Normal, KFMsg::S2S_BROADCAST_TO_WORLD_ACK, KFMsg::S2SBroadcastToWorldAck, HandleBroadcastToWorldAck );
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
            auto gateid = kfmsg->gateid( i );
            auto kfconhash = _gate_conhash.Create( gateid );
            kfconhash->AddHashNode( __STRING__( game ), kfmsg->gameid(), 50 );

            __LOG_INFO__( "add gate=[{}] game=[{}]", KFAppId::ToString( gateid ), KFAppId::ToString( kfmsg->gameid() ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleRemoveGateToWorldReq, KFMsg::S2SRemoveGateToWorldReq )
    {
        auto kfconhash = _gate_conhash.Find( kfmsg->gateid() );
        if ( kfconhash == nullptr )
        {
            return;
        }

        kfconhash->RemoveHashNode( kfmsg->gameid() );

        __LOG_ERROR__( "remove gate=[{}] game=[{}]", KFAppId::ToString( kfmsg->gateid() ), KFAppId::ToString( kfmsg->gameid() ) );
    }

    __KF_NET_EVENT_FUNCTION__( KFWorldModule::OnServerLostGame )
    {
        if ( netdata->_type != __STRING__( game ) )
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
                __LOG_ERROR__( "remove gate=[{}] game=[{}]", KFAppId::ToString( iter.first ), KFAppId::ToString( netdata->_id ) );
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
    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleLoginToWorldReq, KFMsg::S2SLoginToWorldReq )
    {
        auto loginid = __ROUTE_SERVER_ID__;
        auto pblogin = &kfmsg->pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] login world req", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        // 踢掉已经在线的玩家
        KickOnlineToGame( KFMsg::KickByLogin, pblogin->playerid(), _invalid_int, __FUNC_LINE__ );

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
            __LOG_ERROR__( "player[{}:{}] login game failed", pblogin->accountid(), pblogin->playerid() );
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
            __LOG_ERROR__( "player[{}] world verify result[{}] failed", accountid, result );
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

    void KFWorldModule::UpdatePlayerOnline( uint64 accountid, uint64 playerid, uint64 worldid, uint64 gameid )
    {
        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( online );

        __JSON_OBJECT_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __STRING__( accountid ), accountid );
        __JSON_SET_VALUE__( sendjson, __STRING__( playerid ), playerid );
        __JSON_SET_VALUE__( sendjson, __STRING__( world ), worldid );
        __JSON_SET_VALUE__( sendjson, __STRING__( game ), gameid );
        _kf_http_client->MTGet<KFWorldModule>( _url, sendjson );
    }

    void KFWorldModule::KickOnlineToGame( uint32 type, uint64 playerid, uint64 gameid, const char* function, uint32 line )
    {
        __LOG_INFO_FUNCTION__( function, line, "kick=[{}] player=[{}]", type, playerid );

        KFMsg::S2SKickPlayerToGameReq req;
        req.set_type( type );
        req.set_playerid( playerid );
        if ( gameid != _invalid_int )
        {
            _kf_tcp_server->SendNetMessage( gameid, KFMsg::S2S_KICK_PLAYER_TO_GAME_REQ, &req );
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
        auto gameid = __JSON_GET_UINT64__( request, __STRING__( game ) );
        auto playerid = __JSON_GET_UINT64__( request, __STRING__( playerid ) );
        KickOnlineToGame( type, playerid, gameid, __FUNC_LINE__ );
        return _invalid_string;
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
