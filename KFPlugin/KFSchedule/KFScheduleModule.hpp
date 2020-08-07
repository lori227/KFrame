#ifndef __KF_SCHEDULE_MODULE_H__
#define __KF_SCHEDULE_MODULE_H__


/************************************************************************
//    @Module			:    游戏计划任务模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-7-28
************************************************************************/

#include "KFScheduleData.hpp"
#include "KFScheduleInterface.h"
#include "KFZConfig/KFTimeConfig.h"

namespace KFrame
{
    class KFScheduleModule : public KFScheduleInterface
    {
    public:
        KFScheduleModule() = default;
        ~KFScheduleModule();

        virtual void BeforeRun();
        virtual void Run();

        // 逻辑
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 注册计划任务
        virtual void AddSchedule( uint32 timeid, KFModule* module, uint64 objectid,
                                  KFScheduleFunction& startfunction, KFScheduleFunction& finishfunction );
        virtual void RemoveSchedule( KFModule* module );
        virtual void RemoveSchedule( uint32 timeid, KFModule* module );

    protected:
        // 添加计划任务
        void AddSchedule( KFScheduleData* kfdata );

        void RunScheduleUpdate();
        void RunScheduleRegister();
        void RunScheduleRemove();

        // 计算下一次执行时间
        void CalcNextCheckRunTime();

        // 执行计划任务状态更新
        void ExecuteScheduleStart( KFScheduleDataList* schedulelist, KFDate& nowdate );
        void ExecuteScheduleFinish( KFScheduleDataList* schedulelist, KFDate& nowdate );

    private:
        // 下一次执行时间
        uint64 _next_check_run_time = 0u;

        // 需要注册的计划任务
        std::list< KFScheduleData* > _kf_schedule_register;

        // 计划任务
        KFHashMap< uint32, uint32, KFScheduleDataList > _kf_schedule_list;

        // 需要删除的任务
        std::list< std::tuple< uint32, KFModule* > > _kf_schedule_remove;
    };
}



#endif