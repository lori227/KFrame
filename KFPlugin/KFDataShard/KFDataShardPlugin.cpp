#include "KFDataShardPlugin.hpp"
#include "KFDataShardModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDataShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFDataShard );
    }

    void KFDataShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFDataShard );
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

    void KFDataShardPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFDataShardConfig );
    }
}