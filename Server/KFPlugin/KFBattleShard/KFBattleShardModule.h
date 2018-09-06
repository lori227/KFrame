#ifndef __KF_BATTLE_SHARD_MODULE_H__
#define __KF_BATTLE_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    战斗服务器逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-2-22
************************************************************************/

#include "KFrame.h"
#include "KFBattleRoom.h"
#include "KFBattleShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFOption/KFOptionInterface.h"

namespace KFrame
{
    class KFBattleShardModule : public KFBattleShardInterface
    {
    public:
        KFBattleShardModule();
        ~KFBattleShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 有代理服务器注册
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDiscoverBattleProxy );

    protected:
        // 注册战场服务器
        __KF_MESSAGE_FUNCTION__( HandleRegisterServerToBattleShardReq );

        // 收到战场服务器注册
        __KF_MESSAGE_FUNCTION__( HandleTellBattleRegisterToShardReq );

        // 战场服务器断开
        __KF_MESSAGE_FUNCTION__( HandleDisconnectServerToBattleServerReq );

        // 创建一个房间
        __KF_MESSAGE_FUNCTION__( HandleCreateRoomToBattleShardReq );

        // 请求战场服务器开启房间回馈
        __KF_MESSAGE_FUNCTION__( HandleOpenBattleRoomToShardAck );

        // 战场开启
        __KF_MESSAGE_FUNCTION__( HandleOpenRoomToBattleShardAck );

        // 添加阵营
        __KF_MESSAGE_FUNCTION__( HandleAddCampToBattleShardReq );

        // 取消匹配
        __KF_MESSAGE_FUNCTION__( HandleCancelMatchToBattleShardReq );

        // 玩家离开战场
        __KF_MESSAGE_FUNCTION__( HandlePlayerLeaveRoomToBattleShardReq );

        // 玩家进入房间
        __KF_MESSAGE_FUNCTION__( HandlePlayerEnterRoomToBattleShardAck );

        // 战场开始
        __KF_MESSAGE_FUNCTION__( HandleTellRoomStartToBattleShardReq );

        // 游戏开始
        __KF_MESSAGE_FUNCTION__( HandleTellRoomStartToBattleShardAck );

        // 通知玩家房间信息
        __KF_MESSAGE_FUNCTION__( HandleNoticeMatchRoomAck );

        // 玩家登陆战场
        __KF_MESSAGE_FUNCTION__( HandlePlayerLoginRoomToBattleShardReq );

        // 查询战场房间
        __KF_MESSAGE_FUNCTION__( HandleQueryRoomToBattleShardReq );

        // 处理战场结束
        __KF_MESSAGE_FUNCTION__( HandleTellRoomFinishToBattleShardReq );

        // 玩家登陆游戏服务器
        __KF_MESSAGE_FUNCTION__( HandlePlayerOnlineToBattleShardReq );

        // 结算战绩回馈
        __KF_MESSAGE_FUNCTION__( HandlePlayerBattleScoreAck );

        // 战场结算
        __KF_MESSAGE_FUNCTION__( HandleBattleScoreBalanceToShardReq );

        // 上线查询结算
        __KF_MESSAGE_FUNCTION__( HandleOnlieQueryBattleScoreReq );

    private:

        // 查找房间
        KFBattleRoom* FindRoom( uint64 roomid, const char* function, uint32 line );

        // 服务器id
        KFBattleRoom* FindBattleRoomByServerId( uint32 serverid );

        // 删除一个房间
        void RemoveBattleRoom( KFBattleRoom* kfroom );

    private:
        // 战场列表
        KFMap< uint64, uint64, KFBattleRoom > _kf_room_list;

        // 数据库操作
        KFRedisDriver* _battle_redis_driver;
    };
}



#endif