#include "KFLuaPlugin.h"
#include "KFLuaModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLuaPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFLuaPlugin, KFLuaInterface >( new KFLuaModule() );
    }

    void KFLuaPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFLuaPlugin, KFLuaInterface >();
    }

    void KFLuaPlugin::LoadModule()
    {
        _kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
    }
}