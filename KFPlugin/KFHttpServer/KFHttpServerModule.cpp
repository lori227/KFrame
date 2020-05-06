#include "KFHttpServerModule.hpp"
#include "KFHttp/KFHttpServer.h"
#include "KFHttp/KFHttpCommon.h"

namespace KFrame
{
    KFHttpServerModule::KFHttpServerModule()
    {
        _http_server = new KFHttpServer();
    }

    KFHttpServerModule::~KFHttpServerModule()
    {
        delete _http_server;
    }

    void KFHttpServerModule::BeforeRun()
    {
        //////////////////////////////////////////////////////////////////////////////////
        auto kfglogal = KFGlobal::Instance();
        auto kfsetting = KFHttpServerConfig::Instance()->FindHttpSetting( kfglogal->_app_name, kfglogal->_app_type );

        // 计算端口
        kfsetting->_port = _kf_ip_address->CalcListenPort( kfsetting->_port_type, kfsetting->_port, kfglogal->_app_id->GetId() );

        _http_server->Start( kfsetting->_local_ip, kfsetting->_port, kfsetting->_max_thread,
                             kfsetting->_max_queue, kfsetting->_idle_time, kfsetting->_keep_alive );

        // 重新获得外网ip
        auto localip = _kf_ip_address->GetLocalIp();
        kfglogal->_http_server_url = __FORMAT__( "http://{}:{}/", localip, kfsetting->_port );
        __LOG_INFO__( "http server[{}] startup ok", kfglogal->_http_server_url );
    }

    void KFHttpServerModule::Run()
    {
        _http_server->Run();
    }

    void KFHttpServerModule::ShutDown()
    {
        _http_server->ShutDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHttpServerModule::RegisterMethodFunction( const std::string& url, bool sync, KFHttpMethodFunction& function )
    {
        _http_server->RegisterMethonFunction( url, sync, function );
    }

    void KFHttpServerModule::UnRegisterFunction( const std::string& url )
    {
        _http_server->UnRegisterMethonFunction( url );
    }

    const std::string& KFHttpServerModule::GetHttpUrl()
    {
        return KFGlobal::Instance()->_http_server_url;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string KFHttpServerModule::SendCode( uint32 code )
    {
        return KFHttpCommon::SendResponseCode( code );
    }

    uint32 KFHttpServerModule::GetCode( KFJson& json )
    {
        return KFHttpCommon::GetResponseCode( json );
    }

    std::string KFHttpServerModule::SendResponse( KFJson& json )
    {
        return KFHttpCommon::SendResponse( json );
    }

    std::string KFHttpServerModule::SendResponse( KFJson& json, uint32 code )
    {
        return KFHttpCommon::SendResponse( json, code );
    }
}