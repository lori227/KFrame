#include "KFGuildClientPlugin.h"
#include "KFGuildClientModule.h"

namespace KFrame
{
    void KFGuildClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFGuildClient );
    }

    void KFGuildClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGuildClient );
    }

    void KFGuildClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
        __FIND_MODULE__( _kf_mail, KFMailClientInterface );

    }

}