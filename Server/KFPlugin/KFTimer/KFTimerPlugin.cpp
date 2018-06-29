#include "KFTimerPlugin.h"
#include "KFTimerModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTimerPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFTimerPlugin, KFTimerInterface >( new KFTimerModule() );
    }

    void KFTimerPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFTimerPlugin, KFTimerInterface >();
    }

    void KFTimerPlugin::LoadModule()
    {

    }
}