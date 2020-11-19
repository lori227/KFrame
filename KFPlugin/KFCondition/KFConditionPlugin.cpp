#include "KFConditionPlugin.hpp"
#include "KFConditionModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFConditionPlugin::Install()
    {
        __REGISTER_MODULE__( KFCondition );
    }

    void KFConditionPlugin::UnInstall()
    {
        __UN_MODULE__( KFCondition );
    }

    void KFConditionPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
    }

    void KFConditionPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFConditionConfig );
        __KF_ADD_CONFIG__( KFConditionDefineConfig );
        ///////////////////////////////////////////////////////////////////////////////
    }

}