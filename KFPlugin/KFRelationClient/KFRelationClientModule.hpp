#ifndef __KF_RELATION_CLIENT_MODULE_H__
#define __KF_RELATION_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    关系客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFProtocol/KFProtocol.h"
#include "KFGame/KFGameInterface.h"
#include "KFRelationClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFZConfig/KFRelationConfig.hpp"

namespace KFrame
{
    class KFRelationClientModule : public KFRelationClientInterface
    {
    public:
        KFRelationClientModule() = default;
        ~KFRelationClientModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 添加好友度 （目前只支持增加）
        virtual void AddFriendLiness( KFEntity* player, uint64 friendid, uint32 type, uint32 value );
        virtual void AddFriendLinessOnce( KFEntity* player, uint64 friendid, uint32 type, uint32 value );

    protected:
        // 上线查询好友数据
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterQueryRelation );

        // 下线通知
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveUpdateRelation );

        // 属性更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnRelationValueUpdate );
        __KF_UPDATE_STRING_FUNCTION__( OnRelationStringUpdate );

        // 更新关系属性值
        void UpdateValueToRelation( KFEntity* player, KFData* kfdata );

    protected:
        // 查询好友回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryRelationToGameAck );

        // 查询好友申请回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryRelationInviteToGameAck );

        // 申请添加关系请求
        __KF_MESSAGE_FUNCTION__( HandleAddRelationReq );

        // 删除关系请求
        __KF_MESSAGE_FUNCTION__( HandleDelRelationReq );

        // 申请添加好友请求
        __KF_MESSAGE_FUNCTION__( HandleApplyAddRelationToGameAck );

        // 删除好友回馈
        __KF_MESSAGE_FUNCTION__( HandleDelRelationToGameAck );

        // 添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddRelationToGameAck );

        // 更新好友
        __KF_MESSAGE_FUNCTION__( HandleUpdateDataToRelationReq );

        // 关系申请回复
        __KF_MESSAGE_FUNCTION__( HandleReplyRelationInviteReq );

        // 拒绝好友申请设置
        __KF_MESSAGE_FUNCTION__( HandleSetRefuseRelationInviteReq );

        // 更新好感度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessToGameAck );

    private:
        // 发送消息到关系集群
        bool SendToRelation( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到好友
        void SendMessageToRelation( KFEntity* player, const std::string& dataname, uint32 msgid, google::protobuf::Message* message );

        // 发送好友更新消息
        void SendUpdateToRelation( KFEntity* player, const std::string& dataname, MapString& values );

        // 解析好友信息
        void PBRelationToKFData( const KFMsg::PBRelation* pbfriend, KFData* kffriend );

        // 好友申请操作
        void ReplyRelationInvite( KFEntity* player, uint32 operate, const KFRelationSetting* kfsetting );
        void ReplyRelationInvite( KFEntity* player, uint64 playerid, uint32 operate, const KFRelationSetting* kfsetting );
        uint64 ReplyRelationInvite( KFEntity* player, KFData* kfinvite, uint32 operate, const KFRelationSetting* kfsetting );

        // 添加关系
        void AddRelationToRelation( KFEntity* player, uint64 playerid, const std::string& playername, const KFRelationSetting* kfsetting );

        // 查询关系列表
        void SendQueryToRelation( uint64 playerid, const KFRelationSetting* kfsetting );

    private:
        // 玩家组件
        KFComponent* _kf_component = nullptr;
    };
}



#endif