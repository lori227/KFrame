#include "KFTaskPlugin.hpp"
#include "KFTaskModule.hpp"
#include "KFTaskChainModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTaskPlugin::Install()
    {
        __REGISTER_MODULE__( KFTask );
        __REGISTER_MODULE__( KFTaskChain );
    }

    void KFTaskPlugin::UnInstall()
    {
        __UN_MODULE__( KFTask );
        __UN_MODULE__( KFTaskChain );
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
        __KF_ADD_CONFIG__( KFWeightConfig );
        __KF_ADD_CONFIG__( KFTaskChainConfig );
    }
}