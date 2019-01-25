#include "KFGroupClientPlugin.h"
#include "KFGroupClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFGroupClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFGroupClient );
    }

    void KFGroupClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGroupClient );
    }

    void KFGroupClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_match, KFMatchClientInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
    }
}