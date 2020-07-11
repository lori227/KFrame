#include "KFTaskPlugin.hpp"
#include "KFTaskModule.hpp"
#include "KFTaskChainModule.hpp"
#include "KFConfig/KFConfigInterface.h"
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
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_reset, KFResetInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_condition, KFConditionInterface );
        __FIND_MODULE__( _kf_task_chain, KFTaskChainInterface );
    }

    void KFTaskPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFTaskConfig );
        __KF_ADD_CONFIG__( KFTaskChainConfig );
        __KF_ADD_CONFIG__( KFTaskChainRefreshConfig );
    }
}