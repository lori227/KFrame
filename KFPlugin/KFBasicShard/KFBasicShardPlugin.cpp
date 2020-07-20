#include "KFBasicShardPlugin.hpp"
#include "KFBasicShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBasicShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFBasicShard );
    }

    void KFBasicShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFBasicShard );
    }

    void KFBasicShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_basic_database, KFBasicDatabaseInterface );
    }
}