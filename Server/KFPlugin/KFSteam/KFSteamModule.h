#ifndef __KF_STEAM_MODULE_H__
#define __KF_STEAM_MODULE_H__

/************************************************************************
//    @Module			:    steam渠道的差异化业务功能模块
//    @Author           :    Nick Yang
//    @Mail			    :    yangpengbo@laywoodgames.com
//    @Date             :    2018/11/15
************************************************************************/

#include "KFrame.h"
#include "KFSteamInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"

namespace KFrame
{
    class KFSteamModule : public KFSteamInterface
    {
    public:
        KFSteamModule() = default;
        ~KFSteamModule() = default;

        virtual void BeforeRun();

    protected:
        void AddSteamFreeWeekendSchedule();

        void KickFreeWeekendPlayer();

        // 计划任务
        __KF_SCHEDULE_FUNCTION__( OnScheduleKickFreeWeekendPlayer );
    };
}

#endif //__KF_STEAM_MODULE_H__