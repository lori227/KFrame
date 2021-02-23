#ifndef __KF_TIMER_MODULE_H__
#define __KF_TIMER_MODULE_H__

/************************************************************************
//    @Module			:    游戏定时器
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-20
************************************************************************/

#include "KFTimerData.hpp"
#include "KFTimerInterface.h"

namespace KFrame
{
    enum TimerEnum
    {
        // 定时器类型
        Loop = 1,
        Limit = 2,
        //////////////////////////////////////////////////////////
        MaxSlot = 30000,	// 定时器最大槽数量
        SlotTime = 20,		// 每个槽时间
        WheelTime = MaxSlot * SlotTime,	// 600秒一轮, 可以满足大多功能需求只在一圈内完成
    };

    class KFTimerModule : public KFTimerInterface
    {
    public:
        KFTimerModule();
        ~KFTimerModule();

        virtual void BeforeRun();
        virtual void Run();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 注册定时器
        virtual void AddLoopTimer( uint64 object_id, uint64 data_id, uint32 interval_time, uint32 delay_time, KFModule* module, KFTimerFunction& function );
        virtual void AddCountTimer( uint64 object_id, uint64 data_id, uint32 interval_time, uint32 count, KFModule* module, KFTimerFunction& function );
        virtual void AddDelayTimer( uint64 object_id, uint64 data_id, uint32 interval_time, KFModule* module, KFTimerFunction& function );

        // 删除定时器
        virtual void RemoveTimer( uint64 object_id, uint64 data_id, KFModule* module );

    protected:
        void RunTimerUpdate();
        void RunTimerRegister();
        void RunTimerRemove();

        // 查找定时器
        std::shared_ptr<KFTimerData> FindTimerData( uint64 object_id, uint64 data_id, KFModule* module );

        // 添加定时器
        bool AddTimerData( KFModule* module, std::shared_ptr<KFTimerData> timer_data );

        // 删除定时器
        void RemoveRegisterTimerData( uint64 object_id, uint64 data_id, KFModule* module );
        bool RemoveTimerData( uint64 object_id, uint64 data_id, KFModule* module );

        // 从槽里删除定时器
        void RemoveSlotTimer( std::shared_ptr<KFTimerData> timer_data );

        // 添加槽定时器
        void AddSlotTimer( std::shared_ptr<KFTimerData> timer_data );

        // 刷新定时器
        void RunSlotTimer();

        // 执行定时器
        void ExecuteDoneTimer( std::shared_ptr<KFTimerData> timer_data );

    private:
        // 运行消耗时间
        uint64 _total_run_use_time = 0u;

        // 当前槽索引
        uint32 _now_slot = 0u;

        // 定时器列表
        std::shared_ptr<KFTimerData> _slot_timer_data[ TimerEnum::MaxSlot ];

        // 定时器数据
        KFHashMap<KFModule*, KFModuleTimerData> _module_timer_data_list;

        // 需要注册的定时器
        std::list<std::shared_ptr<KFTimerData>> _register_timer_data;

        // 需要删除的定时器
        std::list<std::tuple<uint64, uint64, KFModule*>> _remove_timer_data;
    };
}



#endif