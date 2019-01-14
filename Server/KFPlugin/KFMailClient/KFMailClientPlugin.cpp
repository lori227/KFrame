#include "KFMailClientPlugin.h"
#include "KFMailClientModule.h"

namespace KFrame
{
    void KFMailClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFMailClient );
    }

    void KFMailClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFMailClient );
    }

    void KFMailClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }

}