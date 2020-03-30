#include "KFAccountPlugin.hpp"
#include "KFAccountModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFAccountPlugin::Install()
    {
        __REGISTER_MODULE__( KFAccount );
    }

    void KFAccountPlugin::UnInstall()
    {
        __UN_MODULE__( KFAccount );
    }

    void KFAccountPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
    }

    void KFAccountPlugin::AddConfig()
    {
    }
}