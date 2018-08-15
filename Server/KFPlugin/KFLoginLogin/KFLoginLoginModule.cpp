#include "KFLoginLoginModule.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFLoginLoginModule::KFLoginLoginModule()
    {
        _is_login_close = false;
    }

    KFLoginLoginModule::~KFLoginLoginModule()
    {
    }

    void KFLoginLoginModule::InitModule()
    {
        ///////////////////////////////////////////////////////////////////////////////
    }

    void KFLoginLoginModule::BeforeRun()
    {
        __REGISTER_SHUTDOWN_FUNCTION__( &KFLoginLoginModule::OnDeployShutDownServer );
        __REGISTER_SERVER_LOST_FUNCTION__( &KFLoginLoginModule::OnServerLostHandle );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_REQ, &KFLoginLoginModule::HandleLoginVerifyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_WORLD_VERIFY_ACK, &KFLoginLoginModule::HandleLoginVerifyAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_PLAYERID_ACK, &KFLoginLoginModule::HandleLoginQueryPlayerIdAck );
    }

    void KFLoginLoginModule::BeforeShut()
    {
        __UNREGISTER_SHUTDOWN_FUNCTION__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_WORLD_VERIFY_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_PLAYERID_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFLoginLoginModule::OnServerLostHandle )
    {
        if ( handletype == __KF_STRING__( gate ) )
        {
            KFJson sendjson;
            sendjson.SetValue( __KF_STRING__( appid ), handleid );
            sendjson.SetValue( __KF_STRING__( id ), _kf_zone->GetZone()->_id );
            static auto url = _kf_ip_address->FindPlatformAddress( KFGlobal::Instance()->_app_id ) + __KF_STRING__( lostzone );
            _kf_http_client->StartMTHttpClient( url, sendjson, true );
        }
    }

    __KF_COMMAND_FUNCTION__( KFLoginLoginModule::OnDeployShutDownServer )
    {
        _is_login_close = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFLoginLoginModule::SendLoginVerifyMessage( uint32 gateid, uint32 result, uint32 accountid, uint32 playerid, const std::string& token, const std::string& ip, uint32 port )
    {
        KFMsg::S2SLoginLoginVerifyAck ack;
        ack.set_result( result );
        ack.set_playerid( playerid );
        ack.set_accountid( accountid );
        ack.set_token( token );
        ack.set_ip( ip );
        ack.set_port( port );
        auto ok = _kf_login->SendMessageToGate( gateid, KFMsg::S2S_LOGIN_LOGIN_VERIFY_ACK, &ack );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Login, "player[{}:{}] login verify result[{}] ok!", accountid, playerid, result );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Login, "player[{}:{}] login verify result[{}] failed!", accountid, playerid, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginLoginModule::HandleLoginVerifyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoginVerifyReq );

        auto gateid = __KF_HEAD_ID__( kfguid );
        auto accountid = kfmsg.accountid();
        auto& token = kfmsg.token();

        __LOG_DEBUG__( KFLogEnum::Login, "accountid[{}] login verify", accountid );

        if ( _is_login_close )
        {
            return SendLoginVerifyMessage( gateid, KFMsg::LoginIsClose, accountid, _invalid_int, _invalid_str, _invalid_str, _invalid_int );
        }

        // 访问平台服务器, 验证token
        KFJson sendjson;
        sendjson.SetValue< const std::string& >( __KF_STRING__( token ), kfmsg.token() );
        sendjson.SetValue( __KF_STRING__( gateid ), gateid );
        sendjson.SetValue( __KF_STRING__( accountid ), kfmsg.accountid() );
        sendjson.SetValue( __KF_STRING__( serverid ), KFGlobal::Instance()->_app_id );
        sendjson.SetValue( __KF_STRING__( ip ), kfmsg.ip() );

        auto url = _kf_ip_address->FindPlatformAddress( accountid ) + __KF_STRING__( token );
        _kf_http_client->StartMTHttpClient( url, sendjson, false, this, &KFLoginLoginModule::OnHttpPlatformLoginVerifyCallBack );
    }

    void KFLoginLoginModule::OnHttpPlatformLoginVerifyCallBack( std::string& senddata, std::string& recvdata )
    {
        // 处理验证结果
        KFJson sendjson( senddata );
        KFJson recvjson( recvdata );

        auto gateid = sendjson.GetUInt32( __KF_STRING__( gateid ) );
        auto accountid = sendjson.GetUInt32( __KF_STRING__( accountid ) );

        // 验证失败
        auto retcode = _kf_http_client->GetResponseCode( recvjson );
        if ( retcode != KFMsg::Success )
        {
            return SendLoginVerifyMessage( gateid, retcode, accountid, _invalid_int, _invalid_str, _invalid_str, _invalid_int );
        }

        auto token = recvjson.GetString( __KF_STRING__( token ) );
        auto channel = recvjson.GetUInt32( __KF_STRING__( channel ) );
        if ( accountid == _invalid_int || token.empty() || channel == _invalid_int )
        {
            return SendLoginVerifyMessage( gateid, KFMsg::HttpDataError, accountid, _invalid_int, _invalid_str, _invalid_str, _invalid_int );
        }

        // 查询并创建角色
        KFMsg::S2SLoginQueryPlayerIdReq req;
        req.set_gateid( gateid );
        req.set_token( token );
        req.set_channel( channel );
        req.set_accountid( accountid );
        req.set_zoneid( _kf_zone->GetZone()->_id );
        req.set_logiczoneid( _kf_zone->GetZone()->_logic_id );
        req.set_loginserverid( KFGlobal::Instance()->_app_id );
        auto ok = _kf_data->SendMessageToData( _kf_zone->GetZone()->_id, KFMsg::S2S_LOGIN_QUERY_PLAYERID_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Login, "accountid[{}] query playerid", accountid );
        }
        else
        {
            SendLoginVerifyMessage( gateid, KFMsg::DataSystemBusy, accountid, _invalid_int, _invalid_str, _invalid_str, _invalid_int );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginLoginModule::HandleLoginQueryPlayerIdAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginQueryPlayerIdAck );

        auto& token = kfmsg.token();
        auto playerid = kfmsg.playerid();
        auto gateid = kfmsg.gateid();
        auto accountid = kfmsg.accountid();
        auto loginserverid = kfmsg.loginserverid();
        if ( playerid == _invalid_int )
        {
            return SendLoginVerifyMessage( gateid, KFMsg::CreatePlayerError, _invalid_int, _invalid_int, _invalid_str, _invalid_str, _invalid_int );
        }

        // 通知worldserver
        KFMsg::S2SLoginWorldVerifyReq req;
        req.set_token( token );
        req.set_gateid( gateid );
        req.set_accountid( accountid );
        req.set_playerid( playerid );
        auto ok = _kf_login->SendMessageToWorld( KFMsg::S2S_LOGIN_WORLD_VERIFY_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Login, "accountid[{}] query player[{}] ok!", accountid, playerid );
        }
        else
        {
            SendLoginVerifyMessage( kfguid._head_id, KFMsg::WorldSystemBusy, accountid, _invalid_int, _invalid_str, _invalid_str, _invalid_int );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginLoginModule::HandleLoginVerifyAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginWorldVerifyAck );

        // 通知客户端
        SendLoginVerifyMessage( kfmsg.gateid(), kfmsg.result(), kfmsg.accountid(), kfmsg.playerid(), kfmsg.token(), kfmsg.ip(), kfmsg.port() );
    }
}