#include "KFHttpServerModule.hpp"
#include "KFHttp/KFHttpServer.h"
#include "KFHttp/KFHttpCommon.h"

namespace KFrame
{
    void KFHttpServerModule::BeforeRun()
    {
        //////////////////////////////////////////////////////////////////////////////////
        auto global = KFGlobal::Instance();
        auto setting = FindHttpSetting( global->_app_name, global->_app_type );

        // 计算端口
        setting->_port = _kf_ip_address->CalcListenPort( setting->_port_type, setting->_port, global->_app_id->GetId() );

        _http_server = __NEW_OBJECT__( KFHttpServer );
        _http_server->Start( setting->_local_ip, setting->_port, setting->_max_thread,
                             setting->_max_queue, setting->_idle_time, setting->_keep_alive );

        // 重新获得外网ip
        global->_http_server_url = __FORMAT__( "http://{}:{}/", global->_local_ip, setting->_port );
        __LOG_INFO__( "http server[{}] startup ok", global->_http_server_url );
    }

    void KFHttpServerModule::Run()
    {
        _http_server->Run();
    }

    void KFHttpServerModule::ShutDown()
    {
        _http_server->ShutDown();
        __DELETE_OBJECT__( _http_server );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHttpServerModule::RegisterMethodFunction( const std::string& url, bool sync, KFHttpMethodFunction& function )
    {
        _http_server->RegisterMethodFunction( url, sync, function );
    }

    void KFHttpServerModule::UnRegisterFunction( const std::string& url )
    {
        _http_server->UnRegisterMethodFunction( url );
    }

    const std::string& KFHttpServerModule::GetHttpUrl()
    {
        return KFGlobal::Instance()->_http_server_url;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<KFHttpSetting> KFHttpServerModule::FindHttpSetting( const std::string& app_name, const std::string& app_type )
    {
        for ( auto& iter : KFHttpServerConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_app_name == app_name &&
                    setting->_app_type == app_type )
            {
                return setting;
            }
        }

        return nullptr;
    }
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