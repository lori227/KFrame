#ifndef __KF_GUILD_SHARD_MODULE_H__
#define __KF_GUILD_SHARD_MODULE_H__

/************************************************************************
//    @Moudle			:    帮派数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFrame.h"
#include "KFGuildShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFGuildShardModule : public KFGuildShardInterface
    {
    public:
        KFGuildShardModule();
        ~KFGuildShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    };
}



#endif