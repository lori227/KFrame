#include "KFBasicClientPlugin.hpp"
#include "KFBasicClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFBasicClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFBasicClient );
    }

    void KFBasicClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFBasicClient );
    }

    void KFBasicClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_filter, KFFilterInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }

    void KFBasicClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFZoneConfig );
    }
}