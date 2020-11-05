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
#include "KFXmlReader/KFTeamConfig.hpp"


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
        __KF_MESSAGE_FUNCTION__( HandleTeamCreateReq );

        // 加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamJoinToGameAck );

        // 查询队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamOnlineQueryToGameAck );

        // 请求离开队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamLeaveReq );

        // 请求剔出队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamKickReq );

        // 离开队伍同步
        __KF_MESSAGE_FUNCTION__( HandleTeamLeaveToGameAck );

        // 请求邀请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamInviteReq );

        // 请求申请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamApplyReq );

        // 收到邀请请求
        __KF_MESSAGE_FUNCTION__( HandleTeamTellInviteToGameAck );

        // 收到申请加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamTellApplyToGameAck );

        // 同意加入队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamAgreeReq );

    protected:
        // 更新队伍信息
        void UpdateIntValueToTeam( KFEntity* player, const StringUInt64& values );
        void UpdateStrValueToTeam( KFEntity* player, const StringMap& values );

        // 更新队伍成员信息
        void UpdateMemberIntValueToTeam( KFEntity* player, const StringUInt64& values );
        void UpdateMemberIntValueToTeam( KFEntity* player, const std::string& dataname, uint64 datavalue );
        void UpdateMemberStrValueToTeam( KFEntity* player, const std::string& dataname, const std::string& datavalue );

        // 更新属性到组队
        void EnterLeaveUpdateToTeam( KFEntity* player );

        // 发送队伍邀请到目标
        void SendTeamInviteToTarget( KFEntity* player, KFData* kfteam, uint64 serverid, uint64 playerid );
    private:
        // 玩家组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif