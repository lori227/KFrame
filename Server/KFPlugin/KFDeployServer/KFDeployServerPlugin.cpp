#include "KFDeployServerPlugin.h"
#include "KFDeployServerModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployServerPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFDeployServerPlugin, KFDeployServerInterface >( new KFDeployServerModule() );
    }

    void KFDeployServerPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFDeployServerPlugin, KFDeployServerInterface >();
    }

    void KFDeployServerPlugin::LoadModule()
    {
        _kf_mysql = _kf_plugin_manage->FindModule< KFMySQLInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_http_server = _kf_plugin_manage->FindModule< KFHttpServerInterface >();
    }
}