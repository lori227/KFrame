#include "KFRolePlugin.hpp"
#include "KFRoleModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRolePlugin::Install()
    {
        __REGISTER_MODULE__( KFRole );
    }

    void KFRolePlugin::UnInstall()
    {
        __UN_MODULE__( KFRole );
    }

    void KFRolePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_filter, KFFilterInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }

}