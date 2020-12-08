#ifndef __KF_RELATION_SHARD_MODULE_H__
#define __KF_RELATION_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    好友逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFRelationShardInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFBasicDatabase/KFBasicDatabaseInterface.h"
#include "KFRelationDatabase/KFRelationDatabaseInterface.h"
#include "KFConfig/KFRelationConfig.hpp"

namespace KFrame
{
    class KFRelationShardModule : public KFRelationShardInterface
    {
    public:
        KFRelationShardModule() = default;
        ~KFRelationShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 查询玩家关系信息
        __KF_MESSAGE_FUNCTION__( HandleQueryRelationToRelationReq, KFMsg::S2SQueryRelationToRelationReq );

        // 查询关系申请
        __KF_MESSAGE_FUNCTION__( HandleQueryRelationInviteToRelationReq, KFMsg::S2SQueryRelationInviteToRelationReq );

        // 申请添加好友
        __KF_MESSAGE_FUNCTION__( HandleApplyAddRelationToRelationReq, KFMsg::S2SApplyAddRelationToRelationReq );

        // 删除好友邀请
        __KF_MESSAGE_FUNCTION__( HandleDelRelationInviteToRelationReq, KFMsg::S2SDelRelationInviteToRelationReq );

        // 添加关系
        __KF_MESSAGE_FUNCTION__( HandleAddRelationToRelationReq, KFMsg::S2SAddRelationToRelationReq );

        // 删除关系
        __KF_MESSAGE_FUNCTION__( HandleDelRelationToRelationReq, KFMsg::S2SDelRelationToRelationReq );

        // 设置拒绝
        __KF_MESSAGE_FUNCTION__( HandleRefuseRelationToRelationReq, KFMsg::S2SRefuseRelationToRelationReq );

        // 更新好友度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessToRelationReq, KFMsg::S2SUpdateFriendLinessToRelationReq );

    protected:
        // 信息转换成关系信息
        void MapStringToPBRelation( uint64 playerid, KFMsg::PBRelation* pbrelation, StringMap& basicdata, StringMap& relationdata );

        // 格式化好友key
        std::string FormatRelationKey( uint64 firstid, uint64 secondid, const KFRelationSetting* kfsetting );

        // 添加关系
        void AddRelation( uint64 playerid, uint64 targetid, const KFRelationSetting* kfsetting );

        // 删除关系
        void DelRelation( uint64 playerid, uint64 targetid, const KFRelationSetting* kfsetting );

        // 发送更新好友度
        void SendAddFriendLinessToPlayer( uint64 selfid, uint64 targetid, uint32 friendliness );

        // 更新好友度
        void UpdateFriendLiness( uint64 selfplayerid, uint64 targetplayerid, uint32 type, uint32 addvalue );
    };
}



#endif