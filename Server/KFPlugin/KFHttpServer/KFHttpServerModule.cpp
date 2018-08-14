#include "KFHttpServerModule.h"
#include "KFHttpServerConfig.h"
#include "KFHttp/KFHttpServer.h"
#include "KFHttp/KFHttpCommon.h"

namespace KFrame
{
    KFHttpServerModule::KFHttpServerModule()
    {
        _http_server = __KF_NEW__( KFHttpServer );
    }

    KFHttpServerModule::~KFHttpServerModule()
    {
        __KF_DELETE__( _http_server );
    }

    void KFHttpServerModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_http_server_config, false );
        //////////////////////////////////////////////////////////////////////////////////
    }

    void KFHttpServerModule::BeforeRun()
    {
        __REGISTER_RUN_FUNCTION__( &KFHttpServerModule::Run );
    }

    void KFHttpServerModule::OnceRun()
    {
        auto kfglogal = KFGlobal::Instance();
        auto kfsetting = _kf_http_server_config->FindHttpSetting( kfglogal->_app_name, kfglogal->_app_type );

        // 计算端口
        kfsetting->_port = _kf_ip_address->CalcListenPort( kfsetting->_port_type, kfsetting->_port, kfglogal->_app_id );

        _http_server->Start( kfsetting->_local_ip, kfsetting->_port, kfsetting->_max_thread,
                             kfsetting->_max_queue, kfsetting->_idle_time, kfsetting->_keep_alive );

        // 重新获得外网ip
        auto localip = _kf_ip_address->GetLocalIp();
        _full_url = __FORMAT__( "http://{}:{}/", localip, kfsetting->_port );

        __LOG_INFO__( KFLogEnum::Init, "http server[{}] startup ok!", _full_url );
    }

    void KFHttpServerModule::Run()
    {
        _http_server->Run();
    }

    void KFHttpServerModule::ShutDown()
    {
        _http_server->ShutDown();
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_RUN_FUNCTION__();
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
        return _full_url;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string KFHttpServerModule::SendResponseCode( uint32 code )
    {
        return KFHttpCommon::SendResponseCode( code );
    }

    uint32 KFHttpServerModule::GetResponseCode( KFJson& json )
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

    void KFHttpServerModule::MakeSignature( KFJson& json )
    {
        KFHttpCommon::MakeSignature( json );
    }

    bool KFHttpServerModule::VerifySignature( KFJson& json )
    {
        return KFHttpCommon::VerifySignature( json );
    }
}