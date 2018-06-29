#include "KFClusterServerPlugin.h"
#include "KFClusterServerModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterServerPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFClusterServerPlugin, KFClusterServerInterface >( new KFClusterServerModule() );
    }

    void KFClusterServerPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFClusterServerPlugin, KFClusterServerInterface >();
    }

    void KFClusterServerPlugin::LoadModule()
    {
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
    }
}