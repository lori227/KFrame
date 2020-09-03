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
#include "KFZConfig/KFTeamConfig.hpp"


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
        __KF_UPDATE_DATA_FUNCTION__( OnDataUpdateCallBack );

        // 玩家进入游戏
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterTeamModule );

        // 玩家离开游戏
        __KF_PLAYER_LEAVE_FUNCTION__( OnLeaveTeamModule );

        // 请求创建队伍
        __KF_MESSAGE_FUNCTION__( HandleTeamCreateReq );

    private:
        // 玩家组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif