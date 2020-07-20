#include "KFBasicDatabasePlugin.hpp"
#include "KFBasicDatabaseModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBasicDatabasePlugin::Install()
    {
        __REGISTER_MODULE__( KFBasicDatabase );
    }

    void KFBasicDatabasePlugin::UnInstall()
    {
        __UN_MODULE__( KFBasicDatabase );
    }

    void KFBasicDatabasePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
    }
}