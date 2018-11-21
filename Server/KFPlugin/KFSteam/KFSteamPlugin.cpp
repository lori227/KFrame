#include "KFSteamPlugin.h"
#include "KFSteamModule.h"

namespace KFrame
{
    void KFSteamPlugin::Install()
    {
        __REGISTER_MODULE__( KFSteam );
    }

    void KFSteamPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFSteam );
    }

    void KFSteamPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
    }

}