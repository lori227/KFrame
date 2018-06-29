#include "KFPublicClientPlugin.h"
#include "KFPublicClientModule.h"

namespace KFrame
{
    void KFPublicClientPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFPublicClientPlugin, KFPublicClientInterface >( new KFPublicClientModule() );
    }

    void KFPublicClientPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFPublicClientPlugin, KFPublicClientInterface >();
    }

    void KFPublicClientPlugin::LoadModule()
    {
        _kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
        _kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
        _kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
    }

}