#include "KFRouteProxyPlugin.h"
#include "KFRouteProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRouteProxyPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFRouteProxyPlugin, KFRouteProxyInterface >( new KFRouteProxyModule() );
    }

    void KFRouteProxyPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFRouteProxyPlugin, KFRouteProxyInterface >();
    }

    void KFRouteProxyPlugin::LoadModule()
    {
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_cluster_proxy = _kf_plugin_manage->FindModule< KFClusterProxyInterface >();
    }
}