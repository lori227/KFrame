#include "KFClusterProxyPlugin.h"
#include "KFClusterProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterProxyPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFClusterProxyPlugin, KFClusterProxyInterface >( new KFClusterProxyModule() );
    }

    void KFClusterProxyPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFClusterProxyPlugin, KFClusterProxyInterface >();
    }

    void KFClusterProxyPlugin::LoadModule()
    {
        _kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
    }
}