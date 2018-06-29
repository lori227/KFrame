#include "KFConnectionPlugin.h"
#include "KFConnectionModule.h"

namespace KFrame
{
    void KFConnectionPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFConnectionPlugin, KFConnectionInterface >( new KFConnectionModule() );
    }

    void KFConnectionPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFConnectionPlugin, KFConnectionInterface >();
    }

    void KFConnectionPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
    }

}