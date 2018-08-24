#include "KFKernelPlugin.h"
#include "KFKernelModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFKernelPlugin::Install()
    {
        __REGISTER_MODULE__( KFKernel );
    }

    void KFKernelPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFKernel );
    }

    void KFKernelPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
    }
}