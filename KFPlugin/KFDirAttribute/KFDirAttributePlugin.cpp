#include "KFDirAttributePlugin.hpp"
#include "KFDirAttributeModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDirAttributePlugin::Install()
    {
        __REGISTER_MODULE__( KFDirAttribute );
    }

    void KFDirAttributePlugin::UnInstall()
    {
        __UN_MODULE__( KFDirAttribute );
    }

    void KFDirAttributePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
    }

    void KFDirAttributePlugin::AddConfig()
    {
    }
}