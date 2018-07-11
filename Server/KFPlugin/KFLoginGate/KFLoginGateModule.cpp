#include "KFLoginGateModule.h"
#include "KFProtocol/KFProtocol.h"
#include "KFJson.h"

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
        auto zone = _kf_zone->GetZone();
        auto kfglobal = KFGlobal::Instance();

        KFJson sendjson;
        sendjson.SetValue< const std::string& >( KFField::_address, kfglobal->_interanet_ip );
        sendjson.SetValue( KFField::_port, kfglobal->_listen_port );
        sendjson.SetValue( KFField::_app_id, kfglobal->_app_id );
        sendjson.SetValue( KFField::_id, zone->_id );
        sendjson.SetValue( KFField::_type, zone->_type );
        sendjson.SetValue< const std::string& >( KFField::_name, zone->_name );

        static auto url = _kf_ip_address->FindPlatformAddress( KFGlobal::Instance()->_app_id ) + KFField::_update_zone;
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
            KFLogger::LogSystem( KFLogger::Error, "update login [%u|%s:%u] failed[%u]!",
                                 kfglobal->_app_id, kfglobal->_interanet_ip.c_str(), kfglobal->_listen_port, retcode );
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

        KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] login gate req!",
                            __FUNCTION__, accountid );

        // 注册连接器
        auto ok = _kf_gate->AddConnection( handleid, accountid );
        if ( !ok )
        {
            _kf_gate->RemoveConnection( accountid, 100, __FUNCTION_LINE__ );
            return KFLogger::LogLogin( KFLogger::Error, "[%s] accountid[%u] register connnection failed!",
                                       __FUNCTION__, accountid );
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
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u:%s] login verifty!",
                                __FUNCTION__, accountid, ip.c_str() );
        }
        else
        {
            // 发送错误
            _kf_display->SendToClient( handleid, KFMsg::LoginSystemBusy );
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u:%s] login failed!",
                                __FUNCTION__, accountid, ip.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginGateModule::HandleLoginVerifyAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoginVerifyAck );

        KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] login verifty result[%u] ack!",
                            __FUNCTION__, kfmsg.accountid(), kfmsg.result(), kfmsg.result() );

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
                KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] login verifty result[%u] ok!",
                                    __FUNCTION__, kfmsg.accountid(), kfmsg.result(), kfmsg.result() );
            }
            else
            {
                KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] login verifty result[%u] failed!",
                                    __FUNCTION__, kfmsg.accountid(), kfmsg.result(), kfmsg.result() );
            }
        }

        // 1秒以后, 断开连接
        _kf_gate->RemoveConnection( kfmsg.accountid(), 1000, __FUNCTION_LINE__ );
    }


}
