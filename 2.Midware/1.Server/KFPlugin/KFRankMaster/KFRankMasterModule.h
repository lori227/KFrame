#ifndef __KF_RANK_MASTER_MODULE_H__
#define __KF_RANK_MASTER_MODULE_H__

/************************************************************************
//    @Module			:    排行榜刷新模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFRankMasterInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFRankMasterConfig.h"

namespace KFrame
{
    class KFRankMasterModule : public KFRankMasterInterface
    {
    public:
        KFRankMasterModule() = default;
        ~KFRankMasterModule() = default;

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 连接成功
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDisconverShard );

        // 断开连接
        __KF_SERVER_LOST_FUNCTION__( OnServerLostShard );

        // 刷新排行榜定时器
        __KF_TIMER_FUNCTION__( OnTimerRefreshRankData );

        // 计划任务
        __KF_SCHEDULE_FUNCTION__( OnScheduleRefreshRankData );

    protected:
        // 更新排行榜
        __KF_MESSAGE_FUNCTION__( HandleRefreshRankAck );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 启动刷新定时器
        void StartRefreshRankDataTimer();
        void StartRefreshRankDataTimer( const KFRankSetting* kfsetting );

        // 发送排行榜刷新消息
        void SendRefreshRankMessage( uint32 rankid );

    private:
        // hash 列表
        KFConHash _rank_shard_hash;
    };
}

#endif