#ifndef __KF_TEAM_CLIENT_MODULE_H__
#define __KF_TEAM_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    队伍数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-12
************************************************************************/

#include "KFTeamClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFConfig/KFTeamConfig.hpp"


namespace KFrame
{
    class KFTeamClientModule : public KFTeamClientInterface
    {
    public:
        KFTeamClientModule() = default;
        virtual ~KFTeamClientModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 属性更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnTeamIntValueUpdate );
        __KF_UPDATE_STRING_FUNCTION__( OnTeamStrValueUpdate );

        // 玩家进入游戏
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterTeamModule );

        // 玩家离开游戏
        __KF_PLAYER_LEAVE_FUNCTION__( OnLeaveTeamModule );

        // 请求创建队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamCreateReq, KFMsg::MsgTeamCreateReq );

        // 加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamJoinToGameAck, KFMsg::S2STeamJoinToGameAck );

        // 查询队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamOnlineQueryToGameAck, KFMsg::S2STeamOnlineQueryToGameAck );

        // 请求离开队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamLeaveReq, KFMsg::MsgTeamLeaveReq );

        // 请求剔出队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamKickReq, KFMsg::MsgTeamKickReq );

        // 离开队伍同步
        __KF_MESSAGE_FUNCTION__( HandleTeamLeaveToGameAck, KFMsg::S2STeamLeaveToGameAck );

        // 请求邀请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamInviteReq, KFMsg::MsgTeamInviteReq );

        // 请求申请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamApplyReq, KFMsg::MsgTeamApplyReq );

        // 收到邀请请求
        __KF_MESSAGE_FUNCTION__( HandleTeamTellInviteToGameAck, KFMsg::S2STeamTellInviteToGameAck );

        // 收到申请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamTellApplyToGameAck, KFMsg::S2STeamTellInviteToGameAck );

        // 同意加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamAgreeReq, KFMsg::MsgTeamAgreeReq );

    protected:
        // 更新队伍信息
        void UpdateIntValueToTeam( EntityPtr player, const StringUInt64& values );
        void UpdateStrValueToTeam( EntityPtr player, const StringMap& values );

        // 更新队伍成员信息
        void UpdateMemberIntValueToTeam( EntityPtr player, const StringUInt64& values );
        void UpdateMemberIntValueToTeam( EntityPtr player, const std::string& data_name, uint64 data_value );
        void UpdateMemberStrValueToTeam( EntityPtr player, const std::string& data_name, const std::string& data_value );

        // 更新属性到组队
        void EnterLeaveUpdateToTeam( EntityPtr player );

        // 发送队伍邀请到目标
        void SendTeamInviteToTarget( EntityPtr player, DataPtr team_data, uint64 server_id, uint64 player_id );
    private:
        // 玩家组件
        std::shared_ptr<KFComponent> _component = nullptr;
    };
}

#endif