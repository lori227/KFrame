#include "KFMailClientPlugin.hpp"
#include "KFMailClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFMailClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFMailClient );
    }

    void KFMailClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFMailClient );
    }

    void KFMailClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }

    void KFMailClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFMailConfig );
    }
}