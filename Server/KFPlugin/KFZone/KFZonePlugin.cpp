#include "KFZonePlugin.h"
#include "KFZoneModule.h"

namespace KFrame
{
    void KFZonePlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFZonePlugin, KFZoneInterface >( new KFZoneModule() );
    }

    void KFZonePlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFZonePlugin, KFZoneInterface >();
    }

    void KFZonePlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
    }

}