#include "KFDisplayPlugin.h"
#include "KFDisplayModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDisplayPlugin::Install()
    {
        __REGISTER_MODULE__( KFDisplay );
    }

    void KFDisplayPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDisplay );
    }

    void KFDisplayPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );

        auto apptype = KFGlobal::Instance()->_app_type;
        if ( apptype == KFField::_shard )
        {
            __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
        }
        else if ( apptype == KFField::_game )
        {
            __FIND_MODULE__( _kf_player, KFPlayerInterface );
            __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        }
    }
}