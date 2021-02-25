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
    bool KFLoginModule::SendToGate( uint64 gate_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gate_id, msg_id, message );
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
        auto account_id = kfmsg->accountid();
        auto session_id = kfmsg->sessionid();
        static auto _url = _kf_ip_address->GetAuthUrl() + __STRING__( verify );

        __LOG_DEBUG__( "account_id[{}] login verify", account_id );

        // 认证服务器, 验证token
        __JSON_OBJECT_DOCUMENT__( send_json );
        __JSON_SET_VALUE__( send_json, __STRING__( ip ), kfmsg->ip() );
        __JSON_SET_VALUE__( send_json, __STRING__( gateid ), gateid );
        __JSON_SET_VALUE__( send_json, __STRING__( token ), kfmsg->token() );
        __JSON_SET_VALUE__( send_json, __STRING__( sessionid ), session_id );
        __JSON_SET_VALUE__( send_json, __STRING__( accountid ), kfmsg->accountid() );
        __JSON_SET_VALUE__( send_json, __STRING__( zoneid ), setting->_id );
        __JSON_SET_VALUE__( send_json, __STRING__( datazoneid ), setting->_data_id );
        _kf_http_client->MTGet( _url, send_json, this, &KFLoginModule::OnHttpAuthLoginVerifyCallBack );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFLoginModule::OnHttpAuthLoginVerifyCallBack )
    {
        // 处理验证结果
        __JSON_PARSE_STRING__( send_json, send_data );
        __JSON_PARSE_STRING__( recv_json, recv_data );

        auto ip = __JSON_GET_STRING__( send_json, __STRING__( ip ) );
        auto gate_id = __JSON_GET_UINT64__( send_json, __STRING__( gateid ) );
        auto account_id = __JSON_GET_UINT64__( send_json, __STRING__( accountid ) );
        auto session_id = __JSON_GET_UINT64__( send_json, __STRING__( sessionid ) );

        // 验证失败
        auto ret_code = _kf_http_client->GetCode( recv_json );
        if ( ret_code != KFMsg::Ok )
        {
            auto ban_time = __JSON_GET_UINT64__( recv_json, __STRING__( bantime ) );
            return SendLoginAckToGate( ret_code, gate_id, session_id, account_id, ban_time );
        }

        auto token = __JSON_GET_STRING__( recv_json, __STRING__( token ) );
        auto account = __JSON_GET_STRING__( recv_json, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( recv_json, __STRING__( channel ) );
        auto player_id = __JSON_GET_UINT64__( recv_json, __STRING__( playerid ) );
        if ( account_id == _invalid_int || token.empty() || channel == _invalid_int || player_id == _invalid_int )
        {
            return SendLoginAckToGate( KFMsg::HttpDataError, gate_id, session_id, account_id, _invalid_int );
        }

        KFMsg::PBLoginData login_data;
        login_data.set_ip( ip );
        login_data.set_token( token );
        login_data.set_gateid( gate_id );
        login_data.set_channel( channel );
        login_data.set_account( account );
        login_data.set_accountid( account_id );
        login_data.set_sessionid( session_id );
        login_data.set_playerid( player_id );

        // 渠道数据
        if ( __JSON_HAS_MEMBER__( recv_json, __STRING__( channeldata ) ) )
        {
            auto channel_data = login_data.mutable_channeldata();
            auto& channel_data_json = recv_json[ __STRING__( channeldata ) ];
            for ( auto iter = channel_data_json.MemberBegin(); iter != channel_data_json.MemberEnd(); ++iter )
            {
                ( *channel_data )[ iter->name.GetString() ] = iter->value.GetString();
            }
        }

        KFMsg::S2SLoginToWorldReq req;
        req.mutable_pblogin()->CopyFrom( login_data );
        auto ok = SendToWorld( KFMsg::S2S_LOGIN_TO_WORLD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}:{}] send world failed", account, channel, account_id );
            SendLoginAckToGate( KFMsg::LoginWorldSystemBusy, gate_id, session_id, account_id, _invalid_int );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleLoginToLoginAck, KFMsg::S2SLoginToLoginAck )
    {
        __LOG_DEBUG__( "player[{}] login result[{}] ack", kfmsg->accountid(), kfmsg->result() );

        // 通知客户端
        SendLoginAckToGate( kfmsg->result(), kfmsg->gateid(), kfmsg->sessionid(), kfmsg->accountid(), _invalid_int );
    }

    void KFLoginModule::SendLoginAckToGate( uint32 result, uint64 gate_id, uint64 session_id, uint64 account_id, uint64 ban_time )
    {
        KFMsg::S2SLoginToGateAck ack;
        ack.set_result( result );
        ack.set_accountid( account_id );
        ack.set_sessionid( session_id );
        ack.set_bantime( ban_time );
        auto ok = _kf_tcp_server->SendNetMessage( gate_id, KFMsg::S2S_LOGIN_TO_GATE_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] login verify result[{}] failed", account_id, result );
        }
    }
}
