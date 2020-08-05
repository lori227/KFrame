#ifndef __KF_TEAM_SHARD_MODULE_H__
#define __KF_TEAM_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    队伍模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFTeamShardInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFBasicDatabase/KFBasicDatabaseInterface.h"
#include "KFZConfig/KFRankConfig.hpp"
#include "KFZConfig/KFTimeConfig.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    class KFTeamShardModule : public KFTeamShardInterface
    {
    public:
        KFTeamShardModule() = default;
        ~KFTeamShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:

    protected:
        // 连接成功
        void OnRouteConnectCluster( uint64 serverid );
        //////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////

    private:

    };
}

#endif