#include "KFTeamClientPlugin.hpp"
#include "KFTeamClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFTeamClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFTeamClient );
    }

    void KFTeamClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFTeamClient );
    }

    void KFTeamClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }

    void KFTeamClientPlugin::AddConfig()
    {
    }
}