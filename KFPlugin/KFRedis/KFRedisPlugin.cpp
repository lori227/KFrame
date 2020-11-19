#include "KFRedisPlugin.hpp"
#include "KFRedisModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRedisPlugin::Install()
    {
        __REGISTER_MODULE__( KFRedis );
    }

    void KFRedisPlugin::UnInstall()
    {
        __UN_MODULE__( KFRedis );
    }

    void KFRedisPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
    }

    void KFRedisPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRedisConfig );
    }
}