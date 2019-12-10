#include "KFSkinPlugin.hpp"
#include "KFSkinModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFSkinPlugin::Install()
    {
        __REGISTER_MODULE__( KFSkin );
    }

    void KFSkinPlugin::UnInstall()
    {
        __UN_MODULE__( KFSkin );
    }

    void KFSkinPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
    }

    void KFSkinPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFSkinConfig );
    }

}