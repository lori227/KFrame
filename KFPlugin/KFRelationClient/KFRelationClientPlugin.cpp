#include "KFRelationClientPlugin.hpp"
#include "KFRelationClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRelationClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFRelationClient );
    }

    void KFRelationClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFRelationClient );
    }

    void KFRelationClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_filter, KFFilterInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }

    void KFRelationClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRelationConfig );
    }

}