#include "KFTcpServerPlugin.h"
#include "KFTcpServerModule.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTcpServerPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFTcpServerPlugin, KFTcpServerInterface >( new KFTcpServerModule() );
    }

    void KFTcpServerPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFTcpServerPlugin, KFTcpServerInterface >();
    }

    void KFTcpServerPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
        _kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
    }
}