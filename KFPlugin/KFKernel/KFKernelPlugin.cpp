#include "KFKernelPlugin.hpp"
#include "KFKernelModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFKernelPlugin::Install()
    {
        __REGISTER_MODULE__( KFKernel );
    }

    void KFKernelPlugin::UnInstall()
    {
        __UN_MODULE__( KFKernel );
    }

    void KFKernelPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
    }

    void KFKernelPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFDataConfig );
    }
}