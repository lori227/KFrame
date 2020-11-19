#include "KFUnlockPlugin.hpp"
#include "KFUnlockModule.hpp"
#include "KFLoader/KFLoaderInterface.h"

namespace KFrame
{
    void KFUnlockPlugin::Install()
    {
        __REGISTER_MODULE__( KFUnlock );
    }

    void KFUnlockPlugin::UnInstall()
    {
        __UN_MODULE__( KFUnlock );
    }

    void KFUnlockPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_condition, KFConditionInterface );
    }

    void KFUnlockPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFUnlockConfig );
    }

}