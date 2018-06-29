#include "KFOptionPlugin.h"
#include "KFOptionModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFOptionPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFOptionPlugin, KFOptionInterface >( new KFOptionModule() );
    }

    void KFOptionPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFOptionPlugin, KFOptionInterface >();
    }

    void KFOptionPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
    }
}