#include "KFLoginModule.hpp"

namespace KFrame
{
    void KFLoginModule::BeforeRun()
    {
        __REGISTER_TCP_CLIENT_LOST__( &KFLoginModule::OnClientLostWorld );
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFLoginModule::OnClientConnectionWorld );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LOGIN_TO_LOGIN_REQ, &KFLoginModule::HandleLoginToLoginReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LOGIN_TO_LOGIN_ACK, &KFLoginModule::HandleLoginToLoginAck );
    }

    void KFLoginModule::BeforeShut()
    {
        __UN_TCP_CLIENT_LOST__();
        __UN_TCP_CLIENT_CONNECTION__();

        //////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_REQ );
        __UN_MESSAGE__( KFMsg::S2S_LOGIN_TO_LOGIN_ACK );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFLoginModule::OnClientConnectionWorld )
    {
        if ( net_data->_type != __STRING__( world ) )
        {
            return;
        }

        _world_hash.AddHashNode( net_data->_name, net_data->_id, 100 );
        _world_server_id = _world_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
    }

    __KF_NET_EVENT_FUNCTION__( KFLoginModule::OnClientLostWorld )
    {
        if ( net_data->_type != __STRING__( world ) )
        {
            return;
        }

        _world_hash.RemoveHashNode( net_data->_id );
        if ( net_data->_id == _world_server_id )
        {
            _world_server_id = _world_hash.FindHashNode( KFGlobal::Instance()->_app_id->GetId() );
        }
    }

    bool KFLoginModule::SendToWorld( uint32 msg_id, ::google::protobuf::Message* message )
    {
        if ( _world_server_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _world_server_id, msg_id, message );
    }

    // 发送消息到Gate服务器
    bool KFLoginModule::SendToGate( uint64 gateid, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, msg_id, message );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleLoginToLoginReq, KFMsg::S2SLoginToLoginReq )
    {
        auto gateid = __ROUTE_SERVER_ID__;
        auto setting = KFZoneConfig::Instance()->FindSetting( KFGlobal::Instance()->_app_id->GetZoneId() );
        if ( setting == nullptr )
        {
            return __LOG_ERROR__( "can't find zone=[{}] setting", KFGlobal::Instance()->_app_id->GetZoneId() );
        }

        auto& token = kfmsg->token();
        auto account_id = kfmsg->account_id();
        auto sessionid = kfmsg->sessionid();
        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( verify );

        __LOG_DEBUG__( "account_id[{}] login verify", account_id );

        // 认证服务器, 验证token
        __JSON_OBJECT_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __STRING__( ip ), kfmsg->ip() );
        __JSON_SET_VALUE__( sendjson, __STRING__( gateid ), gateid );
        __JSON_SET_VALUE__( sendjson, __STRING__( token ), kfmsg->token() );
        __JSON_SET_VALUE__( sendjson, __STRING__( sessionid ), sessionid );
        __JSON_SET_VALUE__( sendjson, __STRING__( accountid ), kfmsg->account_id() );
        __JSON_SET_VALUE__( sendjson, __STRING__( zoneid ), setting->_id );
        __JSON_SET_VALUE__( sendjson, __STRING__( datazoneid ), setting->_data_id );
        _kf_http_client->MTGet( _url, sendjson, this, &KFLoginModule::OnHttpAuthLoginVerifyCallBack );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFLoginModule::OnHttpAuthLoginVerifyCallBack )
    {
        // 处理验证结果
        __JSON_PARSE_STRING__( sendjson, senddata );
        __JSON_PARSE_STRING__( recvjson, recvdata );

        auto ip = __JSON_GET_STRING__( sendjson, __STRING__( ip ) );
        auto gateid = __JSON_GET_UINT64__( sendjson, __STRING__( gateid ) );
        auto account_id = __JSON_GET_UINT64__( sendjson, __STRING__( accountid ) );
        auto sessionid = __JSON_GET_UINT64__( sendjson, __STRING__( sessionid ) );

        // 验证失败
        auto retcode = _kf_http_client->GetCode( recvjson );
        if ( retcode != KFMsg::Ok )
        {
            auto bantime = __JSON_GET_UINT64__( recvjson, __STRING__( bantime ) );
            return SendLoginAckToGate( retcode, gateid, sessionid, account_id, bantime );
        }

        auto token = __JSON_GET_STRING__( recvjson, __STRING__( token ) );
        auto account = __JSON_GET_STRING__( recvjson, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( recvjson, __STRING__( channel ) );
        auto player_id = __JSON_GET_UINT64__( recvjson, __STRING__( playerid ) );
        if ( account_id == _invalid_int || token.empty() || channel == _invalid_int || player_id == _invalid_int )
        {
            return SendLoginAckToGate( KFMsg::HttpDataError, gateid, sessionid, account_id, _invalid_int );
        }

        KFMsg::PBLoginData pblogin;
        pblogin.set_ip( ip );
        pblogin.set_token( token );
        pblogin.set_gateid( gateid );
        pblogin.set_channel( channel );
        pblogin.set_account( account );
        pblogin.set_accountid( account_id );
        pblogin.set_sessionid( sessionid );
        pblogin.set_playerid( player_id );

        // 渠道数据
        if ( __JSON_HAS_MEMBER__( recvjson, __STRING__( channel_data ) ) )
        {
            auto pbchanneldata = pblogin.mutable_channeldata();
            auto& channel_data = recvjson[ __STRING__( channel_data ) ];
            for ( auto iter = channel_data.MemberBegin(); iter != channel_data.MemberEnd(); ++iter )
            {
                ( *pbchanneldata )[ iter->name.GetString() ] = iter->value.GetString();
            }
        }

        KFMsg::S2SLoginToWorldReq req;
        req.mutable_pblogin()->CopyFrom( pblogin );
        auto ok = SendToWorld( KFMsg::S2S_LOGIN_TO_WORLD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}:{}] send world failed", account, channel, account_id );
            SendLoginAckToGate( KFMsg::LoginWorldSystemBusy, gateid, sessionid, account_id, _invalid_int );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleLoginToLoginAck, KFMsg::S2SLoginToLoginAck )
    {
        __LOG_DEBUG__( "player[{}] login result[{}] ack", kfmsg->account_id(), kfmsg->result() );

        // 通知客户端
        SendLoginAckToGate( kfmsg->result(), kfmsg->gateid(), kfmsg->sessionid(), kfmsg->account_id(), _invalid_int );
    }

    void KFLoginModule::SendLoginAckToGate( uint32 result, uint64 gateid, uint64 sessionid, uint64 account_id, uint64 bantime )
    {
        KFMsg::S2SLoginToGateAck ack;
        ack.set_result( result );
        ack.set_accountid( account_id );
        ack.set_sessionid( sessionid );
        ack.set_bantime( bantime );
        auto ok = _kf_tcp_server->SendNetMessage( gateid, KFMsg::S2S_LOGIN_TO_GATE_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] login verify result[{}] failed", account_id, result );
        }
    }
}
