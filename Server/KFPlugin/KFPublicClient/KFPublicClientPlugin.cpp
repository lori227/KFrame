#include "KFPublicClientPlugin.h"
#include "KFPublicClientModule.h"

namespace KFrame
{
    void KFPublicClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFPublicClient );
    }

    void KFPublicClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFPublicClient );
    }

    void KFPublicClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );

    }

}