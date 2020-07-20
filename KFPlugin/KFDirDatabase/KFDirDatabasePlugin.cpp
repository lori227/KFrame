#include "KFDirDatabasePlugin.hpp"
#include "KFDirDatabaseModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDirDatabasePlugin::Install()
    {
        __REGISTER_MODULE__( KFDirDatabase );
    }

    void KFDirDatabasePlugin::UnInstall()
    {
        __UN_MODULE__( KFDirDatabase );
    }

    void KFDirDatabasePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
    }

    void KFDirDatabasePlugin::AddConfig()
    {
    }
}