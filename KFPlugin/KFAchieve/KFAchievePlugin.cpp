#include "KFAchievePlugin.hpp"
#include "KFAchieveModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFAchievePlugin::Install()
    {
        __REGISTER_MODULE__( KFAchieve );
    }

    void KFAchievePlugin::UnInstall()
    {
        __UN_MODULE__( KFAchieve );
    }

    void KFAchievePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_condition, KFConditionInterface );
    }

    void KFAchievePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFAchieveConfig );
    }
}