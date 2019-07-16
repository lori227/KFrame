#include "KFSchedulePlugin.hpp"
#include "KFScheduleModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFSchedulePlugin::Install()
    {
        __REGISTER_MODULE__( KFSchedule );
    }

    void KFSchedulePlugin::UnInstall()
    {
        __UN_MODULE__( KFSchedule );
    }

    void KFSchedulePlugin::LoadModule()
    {

    }
}