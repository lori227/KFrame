#include "KFAchievePlugin.h"
#include "KFAchieveModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFAchievePlugin::Install()
    {
        __REGISTER_MODULE__( KFAchieve );
    }

    void KFAchievePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFAchieve );
    }

    void KFAchievePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}