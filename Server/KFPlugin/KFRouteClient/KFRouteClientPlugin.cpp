#include "KFRouteClientPlugin.h"
#include "KFRouteClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRouteClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFRouteClient );
    }

    void KFRouteClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRouteClient );
    }

    void KFRouteClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_game, KFGameInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
    }
}