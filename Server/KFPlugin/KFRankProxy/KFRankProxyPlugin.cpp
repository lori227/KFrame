#include "KFRankProxyPlugin.h"
#include "KFRankProxyModule.h"

namespace KFrame
{
    void KFRankProxyPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFRankProxyPlugin, KFRankProxyInterface >( new KFRankProxyModule() );
    }

    void KFRankProxyPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFRankProxyPlugin, KFRankProxyInterface >();
    }

    void KFRankProxyPlugin::LoadModule()
    {
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_cluster_proxy = _kf_plugin_manage->FindModule< KFClusterProxyInterface >();
    }

}