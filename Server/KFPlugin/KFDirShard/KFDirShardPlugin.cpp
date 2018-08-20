#include "KFDirShardPlugin.h"
#include "KFDirShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDirShardPlugin::Install ()
    {
        __REGISTER_MODULE__( KFDirShard );
    }

    void KFDirShardPlugin::UnInstall ()
    {
        __UNREGISTER_MODULE__( KFDirShard );
    }

    void KFDirShardPlugin::LoadModule ()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
    }
}