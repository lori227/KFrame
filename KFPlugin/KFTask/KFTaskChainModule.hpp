#ifndef __KF_TASK_CHAIN_MOUDLE_H__
#define __KF_TASK_CHAIN_MOUDLE_H__

/************************************************************************
//    @Module			:    任务链系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-10-31
************************************************************************/

#include "KFTaskInterface.h"
#include "KFTaskChainInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFTaskChainConfig.hpp"

namespace KFrame
{
    class KFTaskChainModule : public KFTaskChainInterface
    {
    public:
        KFTaskChainModule() = default;
        ~KFTaskChainModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加任务链
        __KF_ADD_ELEMENT_FUNCTION__( AddTaskChainElement );

        // 添加刷新任务链
        __KF_ADD_ELEMENT_FUNCTION__( AddTaskChainrefreshElement );

        // 刷新时间组
        __KF_RESET_FUNCTION__( OnResetRefreshTaskChain );

        // 开启任务链
        bool OpenTaskChain( KFEntity* player, uint32 chainid, uint32 chainindex, uint32 validtime, uint32 refreshid );

        // 开启任务链数据
        bool OpenTaskLogicDataList( KFEntity* player, const KFWeightList<KFTaskData>* taskdatalist, uint32 chainid, uint32 chainindex, uint32 validtime, uint32 refreshid );

        // 清除任务链
        void CleanTaskChain( KFEntity* player, uint32 chainid );

        // 间隔刷新任务链
        void StartRefreshTaskChain( KFEntity* player, const KFTaskChainRefreshSetting* kfrefreshsetting );

        // 任务超时定时器
        __KF_TIMER_FUNCTION__( OnTimerTaskChainRefreshTimeout );

        // 进入游戏检查任务
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterTaskChainModule );

        // 离开游戏删除定时器
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveTaskChainModule );

        // 开启附加任务链
        void OpenExtendChain( KFEntity* player, const UInt32Map& chainlist );

        // 开启刷新循环链
        void OpenRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist );

        // 关闭刷新循环链
        void StopRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist );

        // 删除任务
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveTaskTaskChainModule );
        // 完成任务链任务
        void FinishTaskChain( KFEntity* player, KFData* kftask );
        // 删除任务链
        void RemoveTaskChain( KFEntity* player, KFData* kftask );

    private:
        virtual void BindOpenTaskChainFunction( const std::string& name, KFOpenTaskChainFunction& function );
        virtual void UnBindOpenTaskChainFunction( const std::string& name );

        virtual void BindFinishTaskChainFunction( const std::string& name, KFFinishTaskChainFunction& function );
        virtual void UnBindFinishTaskChainFunction( const std::string& name );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 开启任务链回调
        KFBind< std::string, const std::string&, KFOpenTaskChainFunction > _open_task_chain_function;

        // 关闭任务链回调
        KFBind< std::string, const std::string&, KFFinishTaskChainFunction > _finish_task_chain_function;
    };
}

#endif