#ifndef __KF_RANK_SHARD_MODULE_H__
#define __KF_RANK_SHARD_MODULE_H__

/************************************************************************
//    @Moudle			:    排行榜数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFrame.h"
#include "KFRankShardInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFRankShardModule : public KFRankShardInterface
    {
    public:
        KFRankShardModule();
        ~KFRankShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 客户端连接
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDiscoverClient );

    protected:
        // 更新排行榜
        __KF_MESSAGE_FUNCTION__( HandleJoinRankListReq );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 查询全服排行榜数据
        __KF_MESSAGE_FUNCTION__( HandleQueryWholeRanklistReq );

        // 查询好友排行榜数据
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendRanklistReq );

    private:
        std::string GetRankMainKey( std::string& ranktype );
        std::string MakeRankKey( std::string& mainkey, const std::string& scoretype );
        void AppendCommand( KFRedisDriver* redisdriver, uint32 playerid, std::string& mainkey, const KFMsg::PBScoreData* scores );

        // 批量查询玩家信息
        bool QueryRoleInfoBatch( VectorString& playerinfos, VectorString& playerids, VectorString& fileds );
        // 格式化玩家信息
        void FormatRoleInfoToPB( KFMsg::PBStrings* pbrank, VectorString& playerinfos, int& cursor, VectorString& fileds );
    };
}



#endif