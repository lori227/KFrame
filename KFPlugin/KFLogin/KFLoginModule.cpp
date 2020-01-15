#include "KFLoginModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFLoginModule::BeforeRun()
    {
        __LOOP_TIMER_0__( 10000, 5000, &KFLoginModule::OnTimerZoneRegister );
        __REGISTER_SERVER_LOST__( &KFLoginModule::OnServerLostGate );
        __REGISTER_CLIENT_LOST__( &KFLoginModule::OnClientLostWorld );
        __REGISTER_CLIENT_CONNECTION__( &KFLoginModule::OnClientConnectionWorld );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ZONE_TO_LOGIN_REQ, &KFLoginModule::HandleUpdateZoneToLoginReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_REQ, &KFLoginModule::HandleLoginToLoginReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_ACK, &KFLoginModule::HandleLoginToLoginAck );
    }

    void KFLoginModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_SERVER_LOST__();
        __UN_CLIENT_LOST__();
        __UN_CLIENT_CONNECTION__();

        //////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_ZONE_TO_LOGIN_REQ );
        __UN_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_REQ );
        __UN_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_ACK );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFLoginModule::PrepareRun()
    {
        _auth_redis = _kf_redis->Create( __STRING__( auth ) );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFLoginModule::OnClientConnectionWorld )
    {
        if ( netdata->_type != __STRING__( world ) )
        {
            return;
        }

        _world_hash.AddHashNode( netdata->_name, netdata->_id, 100 );
        _world_server_id = _world_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
    }

    __KF_NET_EVENT_FUNCTION__( KFLoginModule::OnClientLostWorld )
    {
        if ( netdata->_type != __STRING__( world ) )
        {
            return;
        }

        _world_hash.RemoveHashNode( netdata->_id );
        if ( netdata->_id == _world_server_id )
        {
            _world_server_id = _world_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
        }
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
    __KF_TIMER_FUNCTION__( KFLoginModule::OnTimerZoneRegister )
    {
        auto kfsetting = KFZoneConfig::Instance()->ZoneSetting();
        auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( zoneregister );

        // 注册小区信息
        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), kfsetting->_id );
        __JSON_SET_VALUE__( kfjson, __STRING__( name ), kfsetting->_name );
        __JSON_SET_VALUE__( kfjson, __STRING__( recommendzoneid ), KFZoneConfig::Instance()->RecommendZoneId() );

        auto recvdata = _kf_http_client->STGet( _url, kfjson );
        __JSON_PARSE_STRING__( kfresult, recvdata );
        auto retcode = _kf_http_client->GetCode( kfresult );
        if ( retcode == KFMsg::Ok )
        {
            __UN_TIMER_0__();
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleUpdateZoneToLoginReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateZoneToLoginReq );

        auto pbdata = kfmsg.zonedata();
        if ( _zone_data.appid() == _invalid_int ||
                _zone_data.count() > pbdata.count() ||
                ( _zone_data.appid() == pbdata.appid() && _zone_data.count() != pbdata.count() ) )
        {
            // 保存小区数据
            _zone_data.CopyFrom( pbdata );

            // 更新给auth服务器
            auto kfsetting = KFZoneConfig::Instance()->ZoneSetting();
            static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( zoneupdate );

            __JSON_OBJECT_DOCUMENT__( kfjson );
            __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), kfsetting->_id );
            __JSON_SET_VALUE__( kfjson, __STRING__( ip ), _zone_data.ip() );
            __JSON_SET_VALUE__( kfjson, __STRING__( port ), _zone_data.port() );
            __JSON_SET_VALUE__( kfjson, __STRING__( count ), _zone_data.count() );
            __JSON_SET_VALUE__( kfjson, __STRING__( appid ), _zone_data.appid() );
            _kf_http_client->MTGet< KFLoginModule >( _url, kfjson );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFLoginModule::OnServerLostGate )
    {
        if ( _zone_data.appid() != netdata->_id )
        {
            return;
        }

        _zone_data.Clear();

        // 除小区信息到auth
        auto kfsetting = KFZoneConfig::Instance()->ZoneSetting();
        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( zoneremove );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), kfsetting->_id );
        __JSON_SET_VALUE__( kfjson, __STRING__( appid ), netdata->_id );
        _kf_http_client->MTGet< KFLoginModule >( _url, kfjson );
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

        // 本小区的token验证
        auto kftoken = _auth_redis->QueryMap( "hgetall {}:{}:{}", __STRING__( token ), KFZoneConfig::Instance()->ZoneSetting()->_id, accountid );
        if ( !kftoken->_value.empty() )
        {
            auto querytoken = kftoken->_value[ __STRING__( token ) ];
            if ( querytoken == token )
            {
                // 发送到world服务器
                KFMsg::PBLoginData pblogin;
                pblogin.set_token( token );
                pblogin.set_ip( kfmsg.ip() );
                pblogin.set_gateid( gateid );
                pblogin.set_accountid( accountid );
                pblogin.set_sessionid( sessionid );
                pblogin.set_account( kftoken->_value[ __STRING__( account ) ] );
                pblogin.set_channel( KFUtility::ToValue( kftoken->_value[ __STRING__( channel ) ] ) );
                return LoginToWorld( &pblogin );
            }
        }

        // 访问平台服务器, 验证token
        __JSON_OBJECT_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __STRING__( ip ), kfmsg.ip() );
        __JSON_SET_VALUE__( sendjson, __STRING__( gateid ), gateid );
        __JSON_SET_VALUE__( sendjson, __STRING__( token ), kfmsg.token() );
        __JSON_SET_VALUE__( sendjson, __STRING__( sessionid ), sessionid );
        __JSON_SET_VALUE__( sendjson, __STRING__( accountid ), kfmsg.accountid() );
        __JSON_SET_VALUE__( sendjson, __STRING__( zoneid ), KFGlobal::Instance()->_app_id->GetZoneId() );

        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( verify );
        _kf_http_client->MTGet( _url, sendjson, this, &KFLoginModule::OnHttpAuthLoginVerifyCallBack );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFLoginModule::OnHttpAuthLoginVerifyCallBack )
    {
        // 处理验证结果
        __JSON_PARSE_STRING__( sendjson, senddata );
        __JSON_PARSE_STRING__( recvjson, recvdata );

        auto ip = __JSON_GET_STRING__( sendjson, __STRING__( ip ) );
        auto gateid = __JSON_GET_UINT64__( sendjson, __STRING__( gateid ) );
        auto accountid = __JSON_GET_UINT64__( sendjson, __STRING__( accountid ) );
        auto sessionid = __JSON_GET_UINT64__( sendjson, __STRING__( sessionid ) );

        // 验证失败
        auto retcode = _kf_http_client->GetCode( recvjson );
        if ( retcode != KFMsg::Ok )
        {
            auto bantime = __JSON_GET_UINT64__( recvjson, __STRING__( bantime ) );
            return SendLoginAckToGate( retcode, gateid, sessionid, accountid, bantime );
        }

        auto token = __JSON_GET_STRING__( recvjson, __STRING__( token ) );
        auto account = __JSON_GET_STRING__( recvjson, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( recvjson, __STRING__( channel ) );
        if ( accountid == _invalid_int || token.empty() || channel == _invalid_int )
        {
            return SendLoginAckToGate( KFMsg::HttpDataError, gateid, sessionid, accountid, _invalid_int );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // token 保留30天
        static auto _token_expire_time = 3600 * 24 * 30;
        auto tokenkey = __FORMAT__( "{}:{}:{}", __STRING__( token ), KFZoneConfig::Instance()->ZoneSetting()->_id, accountid );

        // 保存验证信息
        StringMap values;
        values[ __STRING__( token ) ] = token;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );
        values[ __STRING__( accountid ) ] = __TO_STRING__( accountid );
        _auth_redis->Append( values, "hmset {}", tokenkey );
        _auth_redis->Append( "expire {} {}", tokenkey, _token_expire_time );
        _auth_redis->Pipeline();
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送到world服务器
        KFMsg::PBLoginData pblogin;
        pblogin.set_ip( ip );
        pblogin.set_token( token );
        pblogin.set_gateid( gateid );
        pblogin.set_channel( channel );
        pblogin.set_account( account );
        pblogin.set_accountid( accountid );
        pblogin.set_sessionid( sessionid );

        // 渠道数据
        if ( __JSON_HAS_MEMBER__( recvjson, __STRING__( channeldata ) ) )
        {
            auto pbchanneldata = pblogin.mutable_channeldata();
            auto& channeldata = recvjson[ __STRING__( channeldata ) ];
            for ( auto iter = channeldata.MemberBegin(); iter != channeldata.MemberEnd(); ++iter )
            {
                ( *pbchanneldata )[ iter->name.GetString() ] = iter->value.GetString();
            }
        }

        LoginToWorld( &pblogin );
    }

    void KFLoginModule::LoginToWorld( const KFMsg::PBLoginData* pblogin )
    {
        // 发送到world服务器
        KFMsg::S2SLoginToWorldReq req;
        req.mutable_pblogin()->CopyFrom( *pblogin );
        auto ok = SendToWorld( KFMsg::S2S_LOGIN_TO_WORLD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] send world failed", pblogin->account(), pblogin->accountid() );
            SendLoginAckToGate( KFMsg::LoginWorldSystemBusy, pblogin->gateid(), pblogin->sessionid(), pblogin->accountid(), _invalid_int );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleLoginToLoginAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginToLoginAck );

        __LOG_DEBUG__( "player[{}] login result[{}] ack", kfmsg.accountid(), kfmsg.result() );

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
            __LOG_ERROR__( "player[{}] login verify result[{}] failed", accountid, result );
        }
    }
}
