#include "KFGuildClientPlugin.h"
#include "KFGuildClientModule.h"

namespace KFrame
{
    void KFGuildClientPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFGuildClientPlugin, KFGuildClientInterface >( new KFGuildClientModule() );
    }

    void KFGuildClientPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFGuildClientPlugin, KFGuildClientInterface >();
    }

    void KFGuildClientPlugin::LoadModule()
    {
        _kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
        _kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
    }

}