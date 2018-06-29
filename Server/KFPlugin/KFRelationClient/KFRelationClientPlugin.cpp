#include "KFRelationClientPlugin.h"
#include "KFRelationClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRelationClientPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFRelationClientPlugin, KFRelationClientInterface >( new KFRelationClientModule() );
    }

    void KFRelationClientPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFRelationClientPlugin, KFRelationClientInterface >();
    }

    void KFRelationClientPlugin::LoadModule()
    {
        _kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
        _kf_route = _kf_plugin_manage->FindModule< KFRouteClientInterface >();
        _kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
    }
}