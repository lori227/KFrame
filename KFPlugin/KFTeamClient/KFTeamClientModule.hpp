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
#include "KFRouteClient/KFRouteClientInterface.h"


namespace KFrame
{
    class KFTeamClientModule : public KFTeamClientInterface
    {
    public:
        KFTeamClientModule() = default;
        ~KFTeamClientModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    private:
        // 玩家组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif