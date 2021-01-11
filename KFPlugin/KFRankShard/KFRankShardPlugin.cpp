#include "KFRankShardPlugin.hpp"
#include "KFRankShardModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    void KFRankShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFRankShard );
    }

    void KFRankShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFRankShard );
    }

    void KFRankShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_basic_database, KFBasicDatabaseInterface );
    }

    void KFRankShardPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRankConfig );
        __KF_ADD_CONFIG__( KFTimeLoopConfig );
    }
}