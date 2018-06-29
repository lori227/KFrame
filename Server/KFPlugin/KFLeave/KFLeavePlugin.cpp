#include "KFEnterPlugin.h"
#include "KFEnterModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFEnterPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFEnterPlugin, KFEnterInterface >( new KFEnterModule() );
    }

    void KFEnterPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFEnterPlugin, KFEnterInterface >();
    }

    void KFEnterPlugin::LoadModule()
    {
        _kf_lua = _kf_plugin_manage->FindModule< KFLuaInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
    }
}