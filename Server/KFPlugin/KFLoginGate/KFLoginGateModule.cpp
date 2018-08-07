#include "KFLoginGateModule.h"
#include "KFProtocol/KFProtocol.h"
#include "KFJson.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    KFLoginGateModule::KFLoginGateModule()
    {
    }

    KFLoginGateModule::~KFLoginGateModule()
    {
    }

    void KFLoginGateModule::InitModule()
    {
    }

    void KFLoginGateModule::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( 0, 60000, &KFLoginGateModule::OnTimerUpdateClientToLogin );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_VERIFY_REQ, &KFLoginGateModule::HandleLoginVerifyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_ACK, &KFLoginGateModule::HandleLoginVerifyAck );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFLoginGateModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_VERIFY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_ACK );
    }

    __KF_TIMER_FUNCTION__( KFLoginGateModule::OnTimerUpdateClientToLogin )
    {
        auto kfzone = _kf_zone->GetZone();
        auto kfglobal = KFGlobal::Instance();
        if ( kfzone->_id == _invalid_int )
        {
            return;
        }

        KFJson sendjson;
        sendjson.SetValue< const std::string& >( __KF_STRING__( address ), kfglobal->_interanet_ip );
        sendjson.SetValue( __KF_STRING__( port ), kfglobal->_listen_port );
        sendjson.SetValue( __KF_STRING__( appid ), kfglobal->_app_id );
        sendjson.SetValue( __KF_STRING__( id ), kfzone->_id );
        sendjson.SetValue( __KF_STRING__( type ), kfzone->_type );
        sendjson.SetValue< const std::string& >( __KF_STRING__( name ), kfzone->_name );

        static auto url = _kf_ip_address->FindPlatformAddress( KFGlobal::Instance()->_app_id ) + __KF_STRING__( updatezone );
        _kf_http_client->StartMTHttpClient( url, sendjson, true, this, &KFLoginGateModule::OnHttpLoginUpdateCallBack );
    }

    void KFLoginGateModule::OnHttpLoginUpdateCallBack( std::string& senddata, std::string& recvdata )
    {
        // 处理验证结果
        KFJson recvjson( recvdata );
        auto retcode = _kf_http_client->GetResponseCode( recvjson );
        if ( retcode != KFMsg::Success )
        {
            auto kfglobal = KFGlobal::Instance();
            __LOG_ERROR__( KFLogEnum::System, "update login [{}|{}:{}] failed[{}]!",
                           kfglobal->_app_id, kfglobal->_interanet_ip, kfglobal->_listen_port, retcode );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLoginGateModule::HandleLoginVerifyReq )
    {
        __PROTO_PARSE__( KFMsg::MsgLoginVerifyReq );

        auto handleid = __KF_HEAD_ID__( kfguid );
        auto accountid = kfmsg.accountid();
        auto& token = kfmsg.token();

        __LOG_DEBUG__( KFLogEnum::Login, "accountid[{}] login gate req!", accountid );

        // 注册连接器
        auto ok = _kf_gate->AddConnection( handleid, accountid );
        if ( !ok )
        {
            _kf_gate->RemoveConnection( accountid, 100, __FUNC_LINE__ );
            return __LOG_ERROR__( KFLogEnum::Login, "accountid[{}] register connection failed!", accountid );
        }

        auto& ip = _kf_gate->GetIp( accountid );

        // 发送到Login服务器验证
        KFMsg::S2SLoginLoginVerifyReq req;
        req.set_accountid( accountid );
        req.set_token( token );
        req.set_ip( ip );
        ok = _kf_gate->SendMessageToLogin( accountid, KFMsg::S2S_LOGIN_LOGIN_VERIFY_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Login, "accountid[{}:{}] login verify!", accountid, ip );
        }
        else
        {
            // 发送错误
            _kf_display->SendToClient( handleid, KFMsg::LoginSystemBusy );
            __LOG_ERROR__( KFLogEnum::Login, "accountid[{}:{}] login failed!", accountid, ip );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginGateModule::HandleLoginVerifyAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoginVerifyAck );
        __LOG_DEBUG__( KFLogEnum::Login, "player[{}:{}] login verify result[{}] ack!", kfmsg.accountid(), kfmsg.playerid(), kfmsg.result() );

        if ( kfmsg.result() != KFMsg::Success )
        {
            _kf_display->SendToClient( kfmsg.accountid(), kfmsg.result() );
        }
        else
        {
            KFMsg::MsgLoginVerifyAck ack;
            ack.set_playerid( kfmsg.playerid() );
            ack.set_token( kfmsg.token() );
            ack.set_ip( kfmsg.ip() );
            ack.set_port( kfmsg.port() );
            auto ok = _kf_gate->SendMessageToClient( kfmsg.accountid(), KFMsg::MSG_LOGIN_VERIFY_ACK, &ack );
            if ( ok )
            {
                __LOG_DEBUG__( KFLogEnum::Login, "player[{}:{}] login verify result[{}] ok!", kfmsg.accountid(), kfmsg.playerid(), kfmsg.result() );
            }
            else
            {
                __LOG_ERROR__( KFLogEnum::Login, "player[{}:{}] login verify result[{}] failed!", kfmsg.accountid(), kfmsg.playerid(), kfmsg.result() );
            }
        }

        // 1秒以后, 断开连接
        _kf_gate->RemoveConnection( kfmsg.accountid(), 1000, __FUNC_LINE__ );
    }


}
