#ifndef __KF_TEAM_SHARD_MODULE_H__
#define __KF_TEAM_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    队伍模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFTeamShardInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFConfig/KFTeamConfig.hpp"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    class KFTeamShardModule : public KFTeamShardInterface
    {
    public:
        KFTeamShardModule() = default;
        ~KFTeamShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 初始化
        void InitTeam( KFEntity* team );
        void UnInitTeam( KFEntity* team );

        // 连接成功
        void OnRouteConnectCluster( uint64 serverid );

        // 更换队长
        void ChangeTeamCaptain( KFEntity* kfteam, uint64 captainid );

        // 设置队长信息
        void SetTeamCaptain( KFEntity* kfteam, DataPtr kfmember, bool update );

        // 发送消息到队伍
        void SendMessageToTeam( KFEntity* kfteam, uint32 msgid, ::google::protobuf::Message* message );
        void SendMessageToMember( DataPtr kfmember, uint32 msgid, ::google::protobuf::Message* message );

        // 通知加入队伍
        void SendJoinTeamToMember( KFEntity* kfteam, DataPtr kfmember );

        // 同步更新属性到客户端
        void SendTeamUpdateDataToMember( KFEntity* kfteam, KFMsg::PBObject& pbobject );

        // 同步添加属性到客户端
        void SendTeamAddDataToMember( KFEntity* kfteam, KFMsg::PBObject& pbobject );

        // 同步删除属性到客户端
        void SendTeamRemoveDataToMember( KFEntity* kfteam, KFMsg::PBObject& pbobject );

        // 删除队员
        void RemoveTeamMember( KFEntity* kfteam, uint64 memberid );
    protected:
        // 创建队伍请求
        __KF_MESSAGE_FUNCTION__( HandleTeamCreateToTeamReq, KFMsg::S2STeamCreateToTeamReq );

        // 加入队伍失败
        __KF_MESSAGE_FUNCTION__( HandleTeamJoinFailedToTeamReq, KFMsg::S2STeamJoinFailedToTeamReq );

        // 上线查询队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamOnlineQueryToTeamReq, KFMsg::S2STeamOnlineQueryToTeamReq );

        // 更新队伍属性
        __KF_MESSAGE_FUNCTION__( HandleTeamIntValueToTeamReq, KFMsg::S2STeamIntValueToTeamReq );
        __KF_MESSAGE_FUNCTION__( HandleTeamStrValueToTeamReq, KFMsg::S2STeamStrValueToTeamReq );

        // 更新队伍成员属性
        __KF_MESSAGE_FUNCTION__( HandleTeamMemberIntValueToTeamReq, KFMsg::S2STeamMemberIntValueToTeamReq );
        __KF_MESSAGE_FUNCTION__( HandleTeamMemberStrValueToTeamReq, KFMsg::S2STeamMemberStrValueToTeamReq );

        // 离开队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamLeaveToTeamReq, KFMsg::S2STeamLeaveToTeamReq );
        // 踢出队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamKickToTeamReq, KFMsg::S2STeamKickToTeamReq );

        // 同意加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamAgreeToTeamReq, KFMsg::S2STeamAgreeToTeamReq );
    private:
        // 队伍上下文组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif