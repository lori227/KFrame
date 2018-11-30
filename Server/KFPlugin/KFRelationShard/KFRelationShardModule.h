#ifndef __KF_RELATION_SHARD_MODULE_H__
#define __KF_RELATION_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    好友逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRelationShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFOption/KFOptionInterface.h"

namespace KFrame
{
    class KFRelationShardModule : public KFRelationShardInterface
    {
    public:
        KFRelationShardModule() = default;
        ~KFRelationShardModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 查询玩家好友信息
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendReq );

        // 查询好友申请
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendInviteReq );

        // 申请添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddFriendInviteReq );

        // 删除好友邀请
        __KF_MESSAGE_FUNCTION__( HandleDelInviteReq );

        // 添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddFriendReq );

        // 删除好友
        __KF_MESSAGE_FUNCTION__( HandleDelFriendReq );

        // 更新好友度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessReq );

        // 添加最近的人
        __KF_MESSAGE_FUNCTION__( HandleAddRecentPlayerDataReq );

        // 查询最近的玩家列表
        __KF_MESSAGE_FUNCTION__( HandleQueryRecentListReq );

        // 处理玩家敬酒请求(被敬酒次数判断)
        __KF_MESSAGE_FUNCTION__( HandlePlayerToastReq );

        // 处理查询总的被敬酒次数请求
        __KF_MESSAGE_FUNCTION__( HandleQueryToastCountReq );

    protected:
        // 计划清理数据库
        __KF_SCHEDULE_FUNCTION__( OnScheduleClearFriendLiness );

    protected:
        // 信息转换成好友信息
        void MapStringToPBPlayer( MapString& values, uint64 friendid, KFMsg::PBRelation* pbrelation );
        void MapStringToPBRelation( MapString& values, KFMsg::PBRelation* pbrelation, bool newadd );

        // 格式化好友key
        std::string FormatFriendKey( const std::string& key, uint64 firstid, uint64 secondid );
        std::string FormatFriendLimitKey( uint64 firstid, uint64 secondid, uint32 type );

        // 发送添加好友消息
        void SendAddFriendToClient( uint64 serverid, MapString& values, uint64 friendid, uint64 playerid );

        // 发送更新好友度
        void SendAddFriendLinessToClient( uint64 selfid, uint64 targetid, uint32 friendliness );

        // 更新好友度
        void UpdateFriendLiness( uint64 selfplayerid, uint64 targetplayerid, uint32 type, uint32 addvalue );

    private:
        // 公共属性数据库
        KFRedisDriver* _public_redis_driver{ nullptr };

        // 关系数据库
        KFRedisDriver* _relation_redis_driver{ nullptr };
    };
}



#endif