#include "KFLoginModule.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFLoginModule::KFLoginModule()
    {
        _is_login_close = false;
        _world_server_id = _invalid_int;
    }

    KFLoginModule::~KFLoginModule()
    {

    }

    void KFLoginModule::InitModule()
    {

    }

    void KFLoginModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFLoginModule::OnClientLostWorld );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFLoginModule::OnClientConnectionWorld );
        __REGISTER_SHUTDOWN_FUNCTION__( &KFLoginModule::OnDeployShutDownServer );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_REQ, &KFLoginModule::HandleLoginVerifyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_FAILED_TO_LOGIN_ACK, &KFLoginModule::HandleLoginFailedAck );
    }

    void KFLoginModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_SHUTDOWN_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_FAILED_TO_LOGIN_ACK );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFLoginModule::IsWorldConnected()
    {
        return _world_server_id != _invalid_int;
    }

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

    bool KFLoginModule::SendMessageToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( _world_server_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _world_server_id, msgid, message );
    }

    // 发送消息到Gate服务器
    bool KFLoginModule::SendMessageToGate( uint32 gateid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, msgid, message );
    }

    __KF_COMMAND_FUNCTION__( KFLoginModule::OnDeployShutDownServer )
    {
        _is_login_close = true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFLoginModule::SendLoginVerifyMessage( uint32 result, uint32 gateid, uint32 sessionid, uint32 accountid )
    {
        KFMsg::S2SLoginLoginVerifyAck ack;
        ack.set_result( result );
        ack.set_accountid( accountid );
        ack.set_sessionid( sessionid );
        auto ok = _kf_tcp_server->SendNetMessage( gateid, KFMsg::S2S_LOGIN_LOGIN_VERIFY_ACK, &ack );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Login, "player[{}] login verify result[{}] ok!", accountid, result );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Login, "player[{}] login verify result[{}] failed!", accountid, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleLoginVerifyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoginVerifyReq );

        auto gateid = __KF_HEAD_ID__( kfguid );
        auto& token = kfmsg.token();
        auto accountid = kfmsg.accountid();
        auto sessionid = kfmsg.sessionid();

        __LOG_DEBUG__( KFLogEnum::Login, "accountid[{}] login verify", accountid );

        if ( _is_login_close )
        {
            return SendLoginVerifyMessage( KFMsg::LoginIsClose, gateid, sessionid, accountid );
        }

        auto kfzone = _kf_zone->GetZone();

        // 访问平台服务器, 验证token
        KFJson sendjson;
        sendjson.SetValue( __KF_STRING__( gateid ), gateid );
        sendjson.SetValue( __KF_STRING__( sessionid ), sessionid );
        sendjson.SetValue( __KF_STRING__( accountid ), kfmsg.accountid() );
        sendjson.SetValue( __KF_STRING__( ip ), kfmsg.ip() );
        sendjson.SetValue( __KF_STRING__( zoneid ), kfzone->_id );
        sendjson.SetValue( __KF_STRING__( logiczoneid ), kfzone->_logic_id );
        sendjson.SetValue( __KF_STRING__( token ), kfmsg.token() );

        auto url = _kf_ip_address->FindAuthAddress() + __KF_STRING__( verify );
        _kf_http_client->StartMTHttpClient( url, sendjson, false, this, &KFLoginModule::OnHttpAuthLoginVerifyCallBack );
    }

    void KFLoginModule::OnHttpAuthLoginVerifyCallBack( std::string& senddata, std::string& recvdata )
    {
        // 处理验证结果
        KFJson sendjson( senddata );
        KFJson recvjson( recvdata );

        auto gateid = sendjson.GetUInt32( __KF_STRING__( gateid ) );
        auto accountid = sendjson.GetUInt32( __KF_STRING__( accountid ) );
        auto sessionid = sendjson.GetUInt32( __KF_STRING__( sessionid ) );

        // 验证失败
        auto retcode = _kf_http_client->GetResponseCode( recvjson );
        if ( retcode != KFMsg::Success )
        {
            return SendLoginVerifyMessage( retcode, gateid, sessionid, accountid );
        }

        auto token = recvjson.GetString( __KF_STRING__( token ) );
        auto channel = recvjson.GetUInt32( __KF_STRING__( channel ) );
        auto playerid = recvjson.GetUInt32( __KF_STRING__( playerid ) );
        if ( accountid == _invalid_int || token.empty() || channel == _invalid_int || playerid == _invalid_int )
        {
            return SendLoginVerifyMessage( KFMsg::HttpDataError, gateid, sessionid, accountid );
        }


        // 通知worldserver
        KFMsg::S2SLoginWorldVerifyReq req;
        req.set_token( token );
        req.set_gateid( gateid );
        req.set_playerid( playerid );
        req.set_accountid( accountid );
        req.set_sessionid( sessionid );

        // 渠道数据
        auto pbchanneldata = req.mutable_channeldata();
        auto channeldata = recvjson[ __KF_STRING__( channeldata ) ];
        for ( auto iter = channeldata.begin(); iter != channeldata.end(); ++iter )
        {
            auto pbdata = pbchanneldata->add_pbstring();
            pbdata->set_name( iter.name() );
            pbdata->set_value( iter->asString() );
        }

        auto ok = SendMessageToWorld( KFMsg::S2S_LOGIN_WORLD_VERIFY_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Login, "accountid[{}] query player[{}] ok!", accountid, playerid );
        }
        else
        {
            SendLoginVerifyMessage( KFMsg::WorldSystemBusy, gateid, sessionid, accountid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginModule::HandleLoginFailedAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginFailedToLoginAck );

        // 通知客户端
        SendLoginVerifyMessage( kfmsg.result(), kfmsg.gateid(), kfmsg.sessionid(), kfmsg.accountid() );
    }

}
