#include "KFRedisPlugin.h"
#include "KFRedisModule.h"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRedisPlugin::Install()
    {
        __REGISTER_MODULE__( KFRedis );
    }

    void KFRedisPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRedis );
    }

    void KFRedisPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}