#include "KFRedisPlugin.hpp"
#include "KFRedisModule.hpp"
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
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }
}