#include "KFMailShardPlugin.hpp"
#include "KFMailShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMailShardPlugin::Install ()
    {
        __REGISTER_MODULE__( KFMailShard );
    }

    void KFMailShardPlugin::UnInstall ()
    {
        __UN_MODULE__( KFMailShard );
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