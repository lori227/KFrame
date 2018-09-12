#include "KFRelationClientPlugin.h"
#include "KFRelationClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRelationClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFRelationClient );
    }

    void KFRelationClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRelationClient );
    }

    void KFRelationClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_mail, KFMailClientInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_public, KFPublicClientInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
    }
}