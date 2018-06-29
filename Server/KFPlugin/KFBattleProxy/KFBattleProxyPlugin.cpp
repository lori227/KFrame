#include "KFBattleProxyPlugin.h"
#include "KFBattleProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBattleProxyPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFBattleProxyPlugin, KFBattleProxyInterface >( new KFBattleProxyModule() );
    }

    void KFBattleProxyPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFBattleProxyPlugin, KFBattleProxyInterface >();
    }

    void KFBattleProxyPlugin::LoadModule()
    {
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_cluster_proxy = _kf_plugin_manage->FindModule< KFClusterProxyInterface >();
    }
}