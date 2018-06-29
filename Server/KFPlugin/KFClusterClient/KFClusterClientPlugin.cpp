#include "KFClusterClientPlugin.h"
#include "KFClusterClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterClientPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFClusterClientPlugin, KFClusterClientInterface >( new KFClusterClientModule() );
    }

    void KFClusterClientPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFClusterClientPlugin, KFClusterClientInterface >();
    }

    void KFClusterClientPlugin::LoadModule()
    {
        _kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
    }
}