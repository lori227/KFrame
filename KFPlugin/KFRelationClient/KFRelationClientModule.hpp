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
#include "KFRelationClientInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFPublicClient/KFPublicClientInterface.h"

namespace KFrame
{
    class KFComponent;
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
        void OnEnterQueryFriend( KFEntity* player );

        // 下线通知
        void OnLeaveUpdateFriend( KFEntity* player );

        // 属性更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnRelationValueUpdate );
        __KF_UPDATE_STRING_FUNCTION__( OnRelationStringUpdate );

    protected:
        // 查询好友回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendToGameAck );

        // 查询好友申请回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendInviteToGameAck );

        // 申请添加好友请求
        __KF_MESSAGE_FUNCTION__( HandleAddFriendInviteReq );

        // 申请添加好友请求
        __KF_MESSAGE_FUNCTION__( HandleAddFriendInviteToGameAck );

        // 删除好友请求
        __KF_MESSAGE_FUNCTION__( HandleDelFriendReq );

        // 删除好友回馈
        __KF_MESSAGE_FUNCTION__( HandleDelFriendToGameAck );

        // 好友申请回复
        __KF_MESSAGE_FUNCTION__( HandleReplyInviteReq );

        // 添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddFriendToGameAck );

        // 更新好友
        __KF_MESSAGE_FUNCTION__( HandleUpdateDataToFriendReq );

        // 拒绝好友申请设置
        __KF_MESSAGE_FUNCTION__( HandleSetRefuseFriendInviteReq );

        // 更新好感度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessToGameAck );

    private:
        // 发送消息到关系集群
        bool SendToRelation( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到关系属性
        bool SendToRelation( uint64 playerid, KFData* kfrelation, uint32 msgid, google::protobuf::Message* message );

        // 发送消息到好友
        void SendMessageToFriend( KFEntity* player, uint32 msgid, google::protobuf::Message* message );

        // 解析好友信息
        void PBRelationToKFData( const KFMsg::PBRelation* pbfriend, KFData* kffriend );

        // 好友申请操作
        void ReplyFriendInvite( KFEntity* player, uint32 operate );
        void ReplyFriendInvite( KFEntity* player, uint64 playerid, uint32 operate );
        uint64 ReplyFriendInvite( KFEntity* player, KFData* kfinvite, uint32 operate );

        // 添加好友
        void ProcessAddFriend( KFEntity* player, KFData* kfinvite );

        // 发送好友更新消息
        void SendUpdateToFriend( KFEntity* player, MapString& values );

        // 获得好友最大设定数量
        uint32 GetMaxFriendCount();

    private:
        // 玩家组件
        KFComponent* _kf_component = nullptr;
    };
}



#endif