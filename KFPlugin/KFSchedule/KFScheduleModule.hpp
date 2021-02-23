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
#include "KFConfig/KFTimeSectionConfig.hpp"

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
        virtual void AddSchedule( uint32 time_section_id, uint64 object_id, KFModule* module,
                                  KFScheduleFunction& start_function, KFScheduleFunction& finish_function );
        virtual void RemoveSchedule( KFModule* module );
        virtual void RemoveSchedule( uint32 time_section_id, KFModule* module );

    protected:
        // 添加计划任务
        void AddSchedule( std::shared_ptr<KFScheduleData> schedule_data );

        void RunScheduleUpdate();
        void RunScheduleRegister();
        void RunScheduleRemove();

        // 计算下一次执行时间
        void CalcNextCheckRunTime();

        // 执行计划任务状态更新
        void ExecuteScheduleStart( std::shared_ptr<KFScheduleDataList> schedule_list, KFDate& now_date );
        void ExecuteScheduleFinish( std::shared_ptr<KFScheduleDataList> schedule_list, KFDate& now_date );

    private:
        // 下一次执行时间
        uint64 _next_check_run_time = 0u;

        // 需要注册的计划任务
        std::list<std::shared_ptr<KFScheduleData>> _schedule_register_list;

        // 计划任务
        KFHashMap<uint32, KFScheduleDataList> _schedule_run_list;

        // 需要删除的任务
        std::list<std::tuple<uint32, KFModule*>> _schedule_remove_list;
    };
}



#endif