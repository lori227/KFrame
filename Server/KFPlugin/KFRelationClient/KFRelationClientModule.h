#ifndef __KF_RELATION_CLIENT_MODULE_H__
#define __KF_RELATION_CLIENT_MODULE_H__

/************************************************************************
//    @Moudle			:    好友客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRelationClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFRelationClientModule : public KFRelationClientInterface
    {
    public:
        KFRelationClientModule();
        ~KFRelationClientModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 添加好友度 （目前只支持增加 --by szz）
        virtual void UpdateFriendLiness( KFEntity* player, uint32 friendid, uint32 operate, uint64 value, uint32 type );

        // 发送消息到关系属性
        virtual bool SendMessageToRelation( KFData* kfrelation, uint32 msgid, google::protobuf::Message* message );
        virtual bool SendMessageToRelation( uint32 serverid, uint32 playerid, uint32 msgid, google::protobuf::Message* message );

        // 发送消息到关系集群
        virtual bool SendMessageToRelation( uint32 msgid, ::google::protobuf::Message* message );
    protected:
        // 上线查询好友数据
        void OnEnterQueryFriend( KFEntity* player );

        // 下线通知
        void OnLeaveUpdateFriend( KFEntity* player );
    protected:
        // 查询好友回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendAck );

        // 查询好友申请回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryInviteAck );

        // 申请添加好友请求
        __KF_MESSAGE_FUNCTION__( HandleAddFriendInviteReq );

        // 申请添加好友请求
        __KF_MESSAGE_FUNCTION__( HandleAddFriendInviteAck );

        // 删除好友请求
        __KF_MESSAGE_FUNCTION__( HandleDelFriendReq );

        // 删除好友回馈
        __KF_MESSAGE_FUNCTION__( HandleDelFriendAck );

        // 好友申请回复
        __KF_MESSAGE_FUNCTION__( HandleReplyInviteReq );

        // 添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddFriendAck );

        // 更新好友
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendReq );

        // 拒绝好友申请设置
        __KF_MESSAGE_FUNCTION__( HandleSetRefuseFriendInviteReq );

        // 更新好感度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessAck );

    private:
        // 解析好友信息
        void PBFriendToKFData( const KFMsg::PBFriend* pbfriend, KFData* kffriend );

        // 好友申请操作
        void ReplyFriendInvite( KFEntity* player, uint32 operate );
        void ReplyFriendInvite( KFEntity* player, uint32 playerid, uint32 operate );
        uint32 ReplyFriendInvite( KFEntity* player, KFData* kfinvite, uint32 operate );

        // 添加好友
        void AddFriend( KFEntity* player, KFData* kfinvite );

        // 属性更新回调
        void OnRelationValueUpdate( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue );
        void OnAddToastCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata );

        void OnRelationStringUpdate( KFEntity* player, KFData* kfdata, const std::string& value );
        // 发送好友更新消息
        void SendUpdateToFriend( KFEntity* player, MapString& values );
    private:
        // 玩家组件
        KFComponent* _kf_component;
    };
}



#endif