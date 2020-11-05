#ifndef __KF_TASK_REFRESH_MOUDLE_H__
#define __KF_TASK_REFRESH_MOUDLE_H__

/************************************************************************
//    @Module			:    任务刷新( 包括任务链 )
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-10-31
************************************************************************/

#include "KFTaskInterface.h"
#include "KFTaskChainInterface.h"
#include "KFTaskRefreshInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFXmlReader/KFTaskRefreshConfig.hpp"

namespace KFrame
{
    class KFTaskRefreshModule : public KFTaskRefreshInterface
    {
    public:
        KFTaskRefreshModule() = default;
        ~KFTaskRefreshModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 开启刷新循环链
        void OpenRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist );

        // 关闭刷新循环链
        void StopRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist );


    protected:
        // 刷新时间组
        __KF_RESET_FUNCTION__( OnResetTaskRefresh );

        // 任务超时定时器
        __KF_TIMER_FUNCTION__( OnTimerTaskRefreshTimeout );

        // 进入游戏检查任务
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterTaskRefreshModule );

        // 离开游戏删除定时器
        __KF_PLAYER_LEAVE_FUNCTION__( OnLeaveTaskRefreshModule );

        // 删除任务
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveTaskTaskRefreshModule );

        // 添加刷新任务
        __KF_ADD_ELEMENT_FUNCTION__( AddTaskRefreshElement );

        // 间隔刷新任务链
        void StartRefreshTaskAndChain( KFEntity* player, const KFTaskRefreshSetting* kfrefreshsetting );

        // 开始刷新随机任务
        void RefreshTask( KFEntity* player, const KFTaskRefreshSetting* kfrefreshsetting );
        void RefreshChain( KFEntity* player, const KFTaskRefreshSetting* kfrefreshsetting );

        // 清空刷新任务
        void CleanRefreshTask( KFEntity* player, uint32 refreshid );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}

#endif