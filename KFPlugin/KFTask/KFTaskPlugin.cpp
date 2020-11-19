#include "KFTaskPlugin.hpp"
#include "KFTaskModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTaskPlugin::Install()
    {
        __REGISTER_MODULE__( KFTask );
    }

    void KFTaskPlugin::UnInstall()
    {
        __UN_MODULE__( KFTask );
    }

    void KFTaskPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_task, KFTaskInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_condition, KFConditionInterface );
    }

    void KFTaskPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFTaskConfig );
    }
}