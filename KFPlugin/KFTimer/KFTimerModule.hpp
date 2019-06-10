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

        // 关闭
        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 获得定时器剩余时间
        virtual uint32 FindLeftTime( const std::string& module, uint64 objectid );

    protected:
        // 注册定时器
        virtual void AddLoopTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 delaytime, KFTimerFunction& function );
        virtual void AddLimitTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 count, KFTimerFunction& function );
        virtual void AddDelayTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function );

        // 删除定时器
        virtual void RemoveTimer( const std::string& module );
        virtual void RemoveTimer( const std::string& module, uint64 objectid );

    protected:
        void RunTimerUpdate();
        void RunTimerRegister();
        void RunTimerRemove();

        // 查找定时器
        KFTimerData* FindTimerData( const std::string& module, uint64 objectid );

        // 添加定时器
        bool AddTimerData( const std::string& module, uint64 objectid, KFTimerData* kfdata );

        // 删除定时器
        bool RemoveTimerData( const std::string& module );
        bool RemoveTimerData( const std::string& module, uint64 objectid );
        void RemoveRegisterData( const std::string& module, uint64 objectid );

        // 从槽里删除定时器
        void RemoveSlotTimer( KFTimerData* kfdata );

        // 添加槽定时器
        void AddSlotTimer( KFTimerData* kfdata );

        // 刷新定时器
        void RunSlotTimer();

    private:
        // 当前槽索引
        uint32 _now_slot;

        // 定时器列表
        KFTimerData* _slot_timer_data[ TimerEnum::MaxSlot ];

        // 上一次刷新时间
        uint64 _last_update_time;

        // 定时器数据
        std::unordered_map< std::string, std::unordered_map< uint64, KFTimerData* > > _kf_timer_data;

        // 需要注册的定时器
        std::list< KFTimerData* > _register_timer_data;

        // 需要删除的定时器
        std::list< std::tuple< std::string, uint64 > > _remove_timer_data;
    };
}



#endif