#include "KFRelationDatabasePlugin.hpp"
#include "KFRelationDatabaseModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRelationDatabasePlugin::Install()
    {
        __REGISTER_MODULE__( KFRelationDatabase );
    }

    void KFRelationDatabasePlugin::UnInstall()
    {
        __UN_MODULE__( KFRelationDatabase );
    }

    void KFRelationDatabasePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
    }

    void KFRelationDatabasePlugin::AddConfig()
    {
    }

}