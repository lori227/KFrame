#include "KFRouteShardPlugin.h"
#include "KFRouteShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRouteShardPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFRouteShardPlugin, KFRouteShardInterface >( new KFRouteShardModule() );
    }

    void KFRouteShardPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFRouteShardPlugin, KFRouteShardInterface >();
    }

    void KFRouteShardPlugin::LoadModule()
    {
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
    }
}