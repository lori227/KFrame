#include "KFBasicAttributePlugin.hpp"
#include "KFBasicAttributeModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBasicAttributePlugin::Install()
    {
        __REGISTER_MODULE__( KFBasicAttribute );
    }

    void KFBasicAttributePlugin::UnInstall()
    {
        __UN_MODULE__( KFBasicAttribute );
    }

    void KFBasicAttributePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
    }
}