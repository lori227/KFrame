#include "KFMailShardPlugin.h"
#include "KFMailShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMailShardPlugin::Install ()
    {
        __REGISTER_MODULE__( KFMailShard );
    }

    void KFMailShardPlugin::UnInstall ()
    {
        __UNREGISTER_MODULE__( KFMailShard );
    }

    void KFMailShardPlugin::LoadModule ()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
    }
}