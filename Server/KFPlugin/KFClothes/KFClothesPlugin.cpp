#include "KFClothesPlugin.h"
#include "KFClothesModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClothesPlugin::Install()
    {
        __REGISTER_MODULE__( KFClothes );
    }

    void KFClothesPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFClothes );
    }

    void KFClothesPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_mail, KFMailClientInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}