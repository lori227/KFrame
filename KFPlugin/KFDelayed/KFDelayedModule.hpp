#ifndef __KF_DELAYED_MODULE_H__
#define __KF_DELAYED_MODULE_H__


/************************************************************************
//    @Module			:    定时功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-13
************************************************************************/

#include "KFDelayedData.hpp"
#include "KFDelayedInterface.h"

namespace KFrame
{
    class KFDelayedModule : public KFDelayedInterface
    {
    public:
        KFDelayedModule() = default;
        virtual ~KFDelayedModule();

        virtual void Run();
        virtual void BeforeRun();

    protected:
        virtual void AddDelayedFunction( uint64 time, uint64 object_id, const int8* data, uint32 size, KFModule* module, KFDelayedFunction& function );
        virtual void AddDelayedFunction( KFTimeData* time_data, uint64 object_id, const int8* data, uint32 size, KFModule* module, KFDelayedFunction& function );
        virtual void RemoveDelayedFunction( KFModule* module );
        virtual void RemoveDelayedFunction( uint64 object_id, KFModule* module );

    protected:
        // 添加定时任务
        void AddDelayedData( std::shared_ptr<KFDelayedData> delay_data );

        // 删除定时任务
        void RemoveDelayedData( uint64 object_id, KFModule* module );

        void RunDelayedUpdate();
        void RunDelayedRegister();
        void RunDelayedRemove();

        // 计算下一次的执行时间
        void CalcNextCheckRunTime();

    private:
        // 下一次执行时间
        uint64 _next_check_run_time = 0u;

        // 需要注册的定时任务
        std::list<std::shared_ptr<KFDelayedData>> _delayed_register_list;

        // 计划任务
        KFHashMap<KFModule*, KFHashMap<uint64, KFDelayedData>> _delayed_data_list;

        // 需要删除的任务
        std::list<std::tuple<uint64, KFModule*>> _delayed_remove_list;

    };
}



#endif