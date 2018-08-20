#ifndef __KF_RANK_SHARD_MODULE_H__
#define __KF_RANK_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    排行榜数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFrame.h"
#include "KFRankShardInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFRankShardConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFRankData
    {
    public:
        KFRankData()
        {
            _rank_id = 0;
            _zone_id = 0;
            _min_rank_score = 0;
        }

    public:
        // 排行榜id
        uint32 _rank_id;

        // 分区id
        uint32 _zone_id;

        // 最小分数
        uint64 _min_rank_score;

        // 排行榜数据
        KFMsg::PBRankDatas _rank_datas;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    class KFRankShardModule : public KFRankShardInterface
    {
    public:
        KFRankShardModule();
        ~KFRankShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 客户端连接
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDiscoverClient );

        // 刷新排行榜定时器
        __KF_TIMER_FUNCTION__( OnTimerRefreshRankData );

        // 计划任务
        void OnScheduleRefreshRankData( uint32 rankid, const char* data, uint32 size );

    protected:
        // 更新排行榜
        __KF_MESSAGE_FUNCTION__( HandleUpdateRankDataReq );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 查询全服排行榜数据
        __KF_MESSAGE_FUNCTION__( HandleQueryRanklistReq );

        // 查询好友排行榜数据
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendRanklistReq );

    protected:
        // 判断是否需要更新排行榜数据
        bool IsNeedUpdateRankData( uint32 rankid, uint32 zoneid, uint64 rankscore );

        // 读取排行榜数据
        void LoadTotalRankData();

        // 保存
        void SaveRankData( KFRankData* kfrankdata );

        // 启动刷新定时器
        void StartRefreshRankDataTimer();
        void StartRefreshRankDataTimer( const KFRankSetting* kfsetting );

        // 刷新排行榜
        void RefreshRankData( uint32 rankid );

        // 加载玩家显示数据
        void LoadPlayerShowData( KFMsg::PBStrings* pbdatas, uint32 playerid );

        // 格式化排行榜数据key
        std::string& FormatRankDataKey( uint32 rankid, uint32 zoneid );
        std::string& FormatRankSortKey( uint32 rankid, uint32 zoneid );

        // 计算zoneid
        uint32 CalcRankZoneId( uint32 playerid, const KFRankSetting* kfsetting );
    private:
        // 排行榜
        KFRedisDriver* _rank_driver;

        // 排行榜列表
        typedef std::pair< uint32, uint32 > RankKey;
        KFMap< RankKey, const RankKey&, KFRankData > _kf_rank_data;
    };
}



#endif