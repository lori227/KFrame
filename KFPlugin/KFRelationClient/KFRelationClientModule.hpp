#ifndef __KF_RELATION_CLIENT_MODULE_H__
#define __KF_RELATION_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    关系客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFRelationClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFConfig/KFRelationConfig.hpp"

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
        virtual void AddFriendliness( EntityPtr player, uint64 friend_id, uint32 type, uint32 value );
        virtual void AddFriendlinessOnce( EntityPtr player, uint64 friend_id, uint32 type, uint32 value );

    protected:
        // 上线查询好友数据
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterQueryRelation );

        // 下线通知
        __KF_PLAYER_LEAVE_FUNCTION__( OnLeaveUpdateRelation );

        // 属性更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnRelationValueUpdate );
        __KF_UPDATE_STRING_FUNCTION__( OnRelationStringUpdate );

        // 更新关系属性值
        void UpdateValueToRelation( EntityPtr player, DataPtr data );

    protected:
        // 查询好友回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryRelationToGameAck, KFMsg::S2SQueryRelationToGameAck );

        // 查询好友申请回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryRelationInviteToGameAck, KFMsg::S2SQueryRelationInviteToGameAck );

        // 申请添加关系请求
        __KF_MESSAGE_FUNCTION__( HandleAddRelationReq, KFMsg::MsgAddRelationReq );

        // 删除关系请求
        __KF_MESSAGE_FUNCTION__( HandleDelRelationReq, KFMsg::MsgDelRelationReq );

        // 申请添加好友请求
        __KF_MESSAGE_FUNCTION__( HandleApplyAddRelationToGameAck, KFMsg::S2SApplyAddRelationToGameAck );

        // 删除好友回馈
        __KF_MESSAGE_FUNCTION__( HandleDelRelationToGameAck, KFMsg::S2SDelRelationToGameAck );

        // 添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddRelationToGameAck, KFMsg::S2SAddRelationToGameAck );

        // 更新好友
        __KF_MESSAGE_FUNCTION__( HandleUpdateIntValueToRelationReq, KFMsg::S2SUpdateIntValueToRelationReq );
        __KF_MESSAGE_FUNCTION__( HandleUpdateStrValueToRelationReq, KFMsg::S2SUpdateStrValueToRelationReq );

        // 关系申请回复
        __KF_MESSAGE_FUNCTION__( HandleReplyRelationInviteReq, KFMsg::MsgReplyRelationInviteReq );

        // 拒绝好友申请设置
        __KF_MESSAGE_FUNCTION__( HandleSetRefuseRelationInviteReq, KFMsg::MsgSetRefuseRelationInviteReq );

        // 更新好感度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessToGameAck, KFMsg::S2SUpdateFriendLinessToGameAck );

    private:
        // 上下限更新关系属性
        void EnterLeaveUpdateToRelation( EntityPtr player, const std::string& relation_name );

        // 解析关系信息
        void PBRelationToKFData( const KFMsg::PBRelation* relation_proto, DataPtr relation_data );

        // 发送消息到好友
        void SendMessageToRelation( EntityPtr player, const std::string& relation_name, uint32 msg_id, google::protobuf::Message* message );

        // 发送好友更新消息
        void UpdateIntValueToRelation( EntityPtr player, const std::string& relation_name, const StringUInt64& values );
        void UpdateIntValueToRelation( EntityPtr player, const std::string& relation_name, const std::string& data_name, uint64 data_value );
        void UpdateStrValueToRelation( EntityPtr player, const std::string& relation_name, const std::string& data_name, const std::string& data_value );

        // 好友申请操作
        void ReplyRelationAllInvite( EntityPtr player, DataPtr invite_record, std::shared_ptr<const KFRelationSetting> setting, uint32 operate );
        void ReplyRelationInvite( EntityPtr player, DataPtr invite_record, std::shared_ptr<const KFRelationSetting> setting, uint32 operate, uint64 player_id );
        uint64 ReplyInvite( EntityPtr player, std::shared_ptr<const KFRelationSetting> setting, uint32 operate, DataPtr invite_data );

        // 添加关系
        void AddRelationToRelation( EntityPtr player, uint64 player_id, const std::string& player_name, DataPtr relation_record, std::shared_ptr<const KFRelationSetting> setting );

        // 通过邀请名查找关系属性配置
        std::shared_ptr<const KFRelationSetting> FindRelationSettingByInviteName( const std::string& name );

    private:
        // 玩家组件
        std::shared_ptr<KFComponent> _component = nullptr;
    };
}



#endif