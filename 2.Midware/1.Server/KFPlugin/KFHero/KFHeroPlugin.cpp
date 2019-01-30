#include "KFHeroPlugin.hpp"
#include "KFHeroModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHeroPlugin::Install()
    {
        __REGISTER_MODULE__( KFHero );
    }

    void KFHeroPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFHero );
    }

    void KFHeroPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }
}