#include "KFDataShardPlugin.hpp"
#include "KFDataShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDataShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFDataShard );
    }

    void KFDataShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDataShard );
    }

    void KFDataShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
        __FIND_MODULE__( _kf_mysql, KFMySQLInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }
}