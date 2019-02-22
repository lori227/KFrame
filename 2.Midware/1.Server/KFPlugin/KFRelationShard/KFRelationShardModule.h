#ifndef __KF_RELATION_SHARD_MODULE_H__
#define __KF_RELATION_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    好友逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFProtocol/KFProtocol.h"
#include "KFRelationShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

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
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendToRelationReq );

        // 查询好友申请
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendInviteToRelationReq );

        // 申请添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddFriendInviteToRelationReq );

        // 删除好友邀请
        __KF_MESSAGE_FUNCTION__( HandleDelInviteToRelationReq );

        // 添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddFriendToRelationReq );

        // 删除好友
        __KF_MESSAGE_FUNCTION__( HandleDelFriendToRelationReq );

        // 更新好友度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessToRelationReq );

    protected:
        // 计划清理数据库
        __KF_SCHEDULE_FUNCTION__( OnScheduleClearFriendLiness );

    protected:
        // 信息转换成好友信息
        void MapStringToPBPlayer( MapString& values, uint64 friendid, KFMsg::PBRelation* pbrelation );
        void MapStringToPBRelation( MapString& values, KFMsg::PBRelation* pbrelation, bool newadd );

        // 格式化好友key
        std::string FormatFriendKey( uint64 firstid, uint64 secondid );
        std::string FormatFriendLimitKey( uint64 firstid, uint64 secondid, uint32 type );

        // 发送添加好友消息
        void SendAddFriendToPlayer( uint64 serverid, MapString& values, uint64 playerid, uint64 friendid );

        // 发送删除好友消息
        void SendDelFriendToPlayer( uint64 serverid, uint64 playerid, uint64 friendid );

        // 发送更新好友度
        void SendAddFriendLinessToPlayer( uint64 selfid, uint64 targetid, uint32 friendliness );

        // 更新好友度
        void UpdateFriendLiness( uint64 selfplayerid, uint64 targetplayerid, uint32 type, uint32 addvalue );

    private:
        // 关系数据库
        KFRedisDriver* _relation_redis_driver = nullptr;

        // 公共数据数据库
        KFRedisDriver* _public_redis_driver = nullptr;
    };
}



#endif