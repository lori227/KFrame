#include "KFSchedulePlugin.hpp"
#include "KFScheduleModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
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
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
    }

    void KFSchedulePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFTimeSectionConfig );
    }
}