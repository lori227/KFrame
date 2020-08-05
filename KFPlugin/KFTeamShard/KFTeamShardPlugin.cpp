#include "KFTeamShardPlugin.hpp"
#include "KFTeamShardModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    void KFTeamShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFTeamShard );
    }

    void KFTeamShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFTeamShard );
    }

    void KFTeamShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_basic_database, KFBasicDatabaseInterface );
    }

    void KFTeamShardPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRankConfig );
    }
}