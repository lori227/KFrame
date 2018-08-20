#ifndef __KF_GROUP_CLIENT_MODULE_H__
#define __KF_GROUP_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    匹配小组客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-4-4
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFGroupClientInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFMatchClient/KFMatchClientInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    class KFGroupClientModule : public KFGroupClientInterface
    {
    public:
        KFGroupClientModule();
        ~KFGroupClientModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();

        // 判断是否是队员
        virtual bool IsGroupMember( KFEntity* player, uint32 playerid );

        // 判断是否是队长
        virtual bool IsGroupCaptain( KFEntity* player, uint32 playerid );

        // 删除一个队员
        virtual void RemoveGroupMember( uint64 groupid, uint32 playerid );

        // 队员数量(没有队伍为1)
        virtual uint32 GroupMemberCount( KFEntity* player );

        // 是否在队伍中
        virtual bool IsInGroup( KFEntity* player );

        // 发送消息到Group
        virtual bool SendMessageToGroup( uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendMessageToGroup( uint64 groupid, uint32 msgid, ::google::protobuf::Message* message );

    protected:
        // 处理邀请玩家匹配
        __KF_MESSAGE_FUNCTION__( HandleInviteMatchGroupReq );

        // 接受到组队邀请
        __KF_MESSAGE_FUNCTION__( HandleReceiveInviteMatchGroupReq );

        // 处理接受邀请
        __KF_MESSAGE_FUNCTION__( HandleReplyInviteMatchGroupReq );

        // 同意邀请组队
        __KF_MESSAGE_FUNCTION__( HandleConsentInviteMatchGroupReq );

        // 队伍数据
        __KF_MESSAGE_FUNCTION__( HandleTellMatchGroupDataAck );

        // 处理添加队员
        __KF_MESSAGE_FUNCTION__( HandleAddMatchGroupMemberAck );

        // 处理删除队员
        __KF_MESSAGE_FUNCTION__( HandleRemoveMatchGroupMemberAck );

        // 申请离开队伍
        __KF_MESSAGE_FUNCTION__( HandleLeaveMatchGroupReq );

        // 剔除玩家
        __KF_MESSAGE_FUNCTION__( HandleKickMatchGroupReq );

        // 离开队伍
        __KF_MESSAGE_FUNCTION__( HandleLeaveMatchGroupAck );

        // 申请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleApplyMatchGroupReq );

        // 申请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleApplyMatchGroupAck );

        // 回复申请组队操作
        __KF_MESSAGE_FUNCTION__( HandleReplyApplyMatchGroupReq );

        // 同意申请最对
        __KF_MESSAGE_FUNCTION__( HandleConsentApplyMatchGroupAck );

        // 更新玩家的基础信息
        __KF_MESSAGE_FUNCTION__( HandleUpdateGroupMemberAck );

        // 请求准备
        __KF_MESSAGE_FUNCTION__( HandleMatchGroupPrepareReq );

        // 更新队伍信息
        __KF_MESSAGE_FUNCTION__( HandleUpdateGroupDataAck );

        // 请求更改队伍模式
        __KF_MESSAGE_FUNCTION__( HandleUpdateGroupMatchReq );

    protected:
        // 登录游戏查询队伍
        void OnEnterQueryMatchGroup( KFEntity* player );

        // 离开游戏更新队伍
        void OnLeaveUpdateMatchGroup( KFEntity* player );

        // 更新队伍属性
        void OnUpdateDataToGroup( KFEntity* player, KFData* kfdata );

        // 更新属性回调
        __KF_UPDATE_STRING_FUNCTION__( OnUpdateStringCallBack );
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataCallBack );

        // 队伍id
        __KF_UPDATE_DATA_FUNCTION__( OnGroupIdUpdateCallBack );

        // 最大数量
        __KF_UPDATE_DATA_FUNCTION__( OnGroupMaxCountUpdateCallBack );

        // 添加队员回调
        __KF_ADD_DATA_FUNCTION__( OnAddGroupMemberCallBack );

        // 删除队员回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveGroupMemberCallBack );
    private:
        // 准备匹配小队
        uint64 PrepareMatchGroup( KFEntity* player, uint32 matchid, uint32 maxcount );

        // 创建匹配小队
        bool CreateMatchGroup( KFEntity* player, uint64 groupid );

        // 格式化队伍成员
        void FormatMatchGroupMember( KFEntity* player, KFMsg::PBObject* pbobject );

        // 离开队伍
        void ProcessLeaveMatchGroup( KFEntity* player );

        // 更新玩家队伍基础信息
        bool UpdateMemberToMatchGroup( uint64 groupid, uint32 playerid, const std::string& dataname, const KFMsg::PBStrings& pbstrings );

        // 更新匹配模式
        void UpdateMatchToGroup( KFEntity* player, uint64 groupid, uint32 matchid, uint32 maxcount );

        // 获得队伍当前数量
        uint32 GetGroupMemberCount( KFData* kfobject );
    private:
        // 匹配队伍成员
        KFData* _kf_group_member;

        // 组件
        KFComponent* _kf_component;
    };
}

#endif