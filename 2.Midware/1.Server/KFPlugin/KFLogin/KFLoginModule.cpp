#include "KFLoginModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFLoginModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFLoginModule::OnServerLostGate );
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFLoginModule::OnClientLostWorld );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFLoginModule::OnClientConnectionWorld );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ZONE_TO_LOGIN_REQ, &KFLoginModule::HandleUpdateZoneToLoginReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_REQ, &KFLoginModule::HandleLoginToLoginReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_ACK, &KFLoginModule::HandleLoginToLoginAck );
    }

    void KFLoginModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();

        //////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ZONE_TO_LOGIN_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_ACK );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFLoginModule::OnClientConnectionWorld )
    {
        if ( servertype != __KF_STRING__( world ) )
        {
            return;
        }

        _world_server_id = serverid;
    }

    __KF_CLIENT_LOST_FUNCTION__( KFLoginModule::OnClientLostWorld )
    {
        if ( servertype != __KF_STRING__( world ) )
        {
            return;
        }

        _world_server_id = _invalid_int;
    }

    bool KFLoginModule::SendToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( _world_server_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _world_server_id, msgid, message );
    }

    // 发送消息到Gate服务器
    bool KFLoginModule::SendToGate( uint64 gateid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, msgid, message );
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleUpdateZoneToLoginReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateZoneToLoginReq );

        KFMsg::S2SUpdateZoneToWorldReq req;
        req.mutable_zonedata()->CopyFrom( kfmsg.zonedata() );
        SendToWorld( KFMsg::S2S_UPDATE_ZONE_TO_WORLD_REQ, &req );
    }

    __KF_SERVER_LOST_FUNCTION__( KFLoginModule::OnServerLostGate )
    {
        if ( handletype == __KF_STRING__( gate ) )
        {
            KFMsg::S2SDisconnectZoneToWorldReq req;
            req.set_appid( handleid );
            SendToWorld( KFMsg::S2S_DISCONNECT_ZONE_TO_WORLD_REQ, &req );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleLoginToLoginReq )
    {
        auto gateid = __ROUTE_SERVER_ID__;
        __PROTO_PARSE__( KFMsg::S2SLoginToLoginReq );

        auto& token = kfmsg.token();
        auto accountid = kfmsg.accountid();
        auto sessionid = kfmsg.sessionid();

        __LOG_DEBUG__( "accountid[{}] login verify", accountid );

        // 访问平台服务器, 验证token
        __JSON_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( ip ), kfmsg.ip() );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( gateid ), gateid );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( token ), kfmsg.token() );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( sessionid ), sessionid );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( accountid ), kfmsg.accountid() );

        static auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( verify );
        _kf_http_client->MTGet( _url, sendjson, this, &KFLoginModule::OnHttpAuthLoginVerifyCallBack );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFLoginModule::OnHttpAuthLoginVerifyCallBack )
    {
        // 处理验证结果
        __JSON_PARSE_STRING__( sendjson, senddata );
        __JSON_PARSE_STRING__( recvjson, recvdata );

        auto gateid = __JSON_GET_UINT64__( sendjson, __KF_STRING__( gateid ) );
        auto accountid = __JSON_GET_UINT64__( sendjson, __KF_STRING__( accountid ) );
        auto sessionid = __JSON_GET_UINT64__( sendjson, __KF_STRING__( sessionid ) );

        // 验证失败
        auto retcode = _kf_http_client->GetCode( recvjson );
        if ( retcode != KFMsg::Ok )
        {
            auto bantime = __JSON_GET_UINT64__( recvjson, __KF_STRING__( bantime ) );
            return SendLoginAckToGate( retcode, gateid, sessionid, accountid, bantime );
        }

        auto token = __JSON_GET_STRING__( recvjson, __KF_STRING__( token ) );
        auto account = __JSON_GET_STRING__( recvjson, __KF_STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( recvjson, __KF_STRING__( channel ) );
        auto playerid = __JSON_GET_UINT64__( recvjson, __KF_STRING__( playerid ) );
        if ( accountid == _invalid_int || token.empty() || channel == _invalid_int || playerid == _invalid_int )
        {
            return SendLoginAckToGate( KFMsg::HttpDataError, gateid, sessionid, accountid, _invalid_int );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送到world服务器
        KFMsg::S2SLoginToWorldReq req;
        auto pblogin = req.mutable_pblogin();

        pblogin->set_token( token );
        pblogin->set_gateid( gateid );
        pblogin->set_channel( channel );
        pblogin->set_account( account );
        pblogin->set_playerid( playerid );
        pblogin->set_accountid( accountid );
        pblogin->set_sessionid( sessionid );

        // 渠道数据
        if ( __JSON_HAS_MEMBER__( recvjson, __KF_STRING__( channeldata ) ) )
        {
            auto pbchanneldata = pblogin->mutable_channeldata();
            auto& channeldata = recvjson[ __KF_STRING__( channeldata ) ];
            for ( auto iter = channeldata.MemberBegin(); iter != channeldata.MemberEnd(); ++iter )
            {
                ( *pbchanneldata )[ iter->name.GetString() ] = iter->value.GetString();
            }
        }

        auto ok = SendToWorld( KFMsg::S2S_LOGIN_TO_WORLD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}:{}] send world failed!", account, accountid, playerid );
            SendLoginAckToGate( KFMsg::LoginWorldSystemBusy, gateid, sessionid, accountid, _invalid_int );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleLoginToLoginAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginToLoginAck );

        __LOG_DEBUG__( "player[{}] login result[{}] ack!", kfmsg.accountid(), kfmsg.result() );

        // 通知客户端
        SendLoginAckToGate( kfmsg.result(), kfmsg.gateid(), kfmsg.sessionid(), kfmsg.accountid(), _invalid_int );
    }

    void KFLoginModule::SendLoginAckToGate( uint32 result, uint64 gateid, uint64 sessionid, uint64 accountid, uint64 bantime )
    {
        KFMsg::S2SLoginToGateAck ack;
        ack.set_result( result );
        ack.set_accountid( accountid );
        ack.set_sessionid( sessionid );
        ack.set_bantime( bantime );
        auto ok = _kf_tcp_server->SendNetMessage( gateid, KFMsg::S2S_LOGIN_TO_GATE_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] login verify result[{}] failed!", accountid, result );
        }
    }
}
