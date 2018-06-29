#include "KFSchedulePlugin.h"
#include "KFScheduleModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFSchedulePlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFSchedulePlugin, KFScheduleInterface >( new KFScheduleModule() );
    }

    void KFSchedulePlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFSchedulePlugin, KFScheduleInterface >();
    }

    void KFSchedulePlugin::LoadModule()
    {

    }
}