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
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFZConfig/KFRelationConfig.hpp"

namespace KFrame
{
    class KFRelationShardModule : public KFRelationShardInterface
    {
    public:
        KFRelationShardModule() = default;
        ~KFRelationShardModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 查询玩家关系信息
        __KF_MESSAGE_FUNCTION__( HandleQueryRelationToRelationReq );

        // 查询关系申请
        __KF_MESSAGE_FUNCTION__( HandleQueryRelationInviteToRelationReq );

        // 申请添加好友
        __KF_MESSAGE_FUNCTION__( HandleApplyAddRelationToRelationReq );

        // 删除好友邀请
        __KF_MESSAGE_FUNCTION__( HandleDelRelationInviteToRelationReq );

        // 添加关系
        __KF_MESSAGE_FUNCTION__( HandleAddRelationToRelationReq );

        // 删除关系
        __KF_MESSAGE_FUNCTION__( HandleDelRelationToRelationReq );

        // 更新好友度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessToRelationReq );

    protected:
        // 信息转换成好友信息
        void MapStringToPBPlayer( MapString& values, uint64 friendid, KFMsg::PBRelation* pbrelation );
        void MapStringToPBRelation( MapString& values, KFMsg::PBRelation* pbrelation, bool newadd );

        // 格式化好友key
        std::string FormatRelationKey( uint64 firstid, uint64 secondid, const KFRelationSetting* kfsetting );

        // 添加关系
        void AddRelation( uint64 playerid, uint64 relationid, const KFRelationSetting* kfsetting );

        // 删除关系
        void DelRelation( uint64 playerid, uint64 relationid, const KFRelationSetting* kfsetting );

        // 发送更新好友度
        void SendAddFriendLinessToPlayer( uint64 selfid, uint64 targetid, uint32 friendliness );

        // 更新好友度
        void UpdateFriendLiness( uint64 selfplayerid, uint64 targetplayerid, uint32 type, uint32 addvalue );

    private:
        // 关系数据库
        KFRedisDriver* _relation_redis_driver = nullptr;

        // 公共数据数据库
        KFRedisDriver* _public_redis = nullptr;
    };
}



#endif