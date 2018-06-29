#include "KFResetPlugin.h"
#include "KFResetModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFResetPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFResetPlugin, KFResetInterface >( new KFResetModule() );
    }

    void KFResetPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFResetPlugin, KFResetInterface >();
    }

    void KFResetPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
    }
}