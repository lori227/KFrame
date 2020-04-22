#include "KFRelationAttributePlugin.hpp"
#include "KFRelationAttributeModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRelationAttributePlugin::Install()
    {
        __REGISTER_MODULE__( KFRelationAttribute );
    }

    void KFRelationAttributePlugin::UnInstall()
    {
        __UN_MODULE__( KFRelationAttribute );
    }

    void KFRelationAttributePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
    }

    void KFRelationAttributePlugin::AddConfig()
    {
    }

}