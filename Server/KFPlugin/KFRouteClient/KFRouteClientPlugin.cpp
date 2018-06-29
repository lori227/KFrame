#include "KFRouteClientPlugin.h"
#include "KFRouteClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRouteClientPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFRouteClientPlugin, KFRouteClientInterface >( new KFRouteClientModule() );
    }

    void KFRouteClientPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFRouteClientPlugin, KFRouteClientInterface >();
    }

    void KFRouteClientPlugin::LoadModule()
    {
        _kf_game = _kf_plugin_manage->FindModule< KFGameInterface >();
        _kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
    }
}