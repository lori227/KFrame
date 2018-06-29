#include "KFMailProxyPlugin.h"
#include "KFMailProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMailProxyPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFMailProxyPlugin, KFMailProxyInterface >( new KFMailProxyModule() );
    }

    void KFMailProxyPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFMailProxyPlugin, KFMailProxyInterface >();
    }

    void KFMailProxyPlugin::LoadModule()
    {
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_cluster_proxy = _kf_plugin_manage->FindModule< KFClusterProxyInterface >();
    }
}