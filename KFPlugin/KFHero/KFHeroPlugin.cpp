#include "KFHeroPlugin.hpp"
#include "KFHeroModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHeroPlugin::Install()
    {
        __REGISTER_MODULE__( KFHero );
    }

    void KFHeroPlugin::UnInstall()
    {
        __UN_MODULE__( KFHero );
    }

    void KFHeroPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }

    void KFHeroPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFHeroConfig );
    }
}