#ifndef __KF_TIMER_MANAGE_H__
#define __KF_TIMER_MANAGE_H__

#include "KFrame.h"
#include "KFTimerData.h"

namespace KFrame
{
    enum TimerEnum
    {
        // 定时器类型
        Loop = 1,
        Limit = 2,
        //////////////////////////////////////////////////////////

        MaxSlot = 1000,	// 定时器最大槽数量
        SlotTime = 1,	// 每个槽时间
        WheelTime = MaxSlot * SlotTime,
    };

    // http://www.cnblogs.com/zhanghairong/p/3757656.html
    class KFTimerManage : public KFSingleton< KFTimerManage >
    {
    public:
        KFTimerManage();
        ~KFTimerManage();

        void Initialize();

        // 更新
        void RunUpdate();

        // 关闭
        void ShutDown();

        // 注册定时器
        void RegisterLoopTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function );
        void RegisterLimitTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 count, KFTimerFunction& function );
        void RegisterDelayTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function );


        // 删除定时器
        bool UnRegisterTimer( const std::string& module );
        bool UnRegisterTimer( const std::string& module, uint64 objectid );

        // 查找剩余时间
        uint32 FindLeftTime( const std::string& module, uint64 objectid );

    protected:
        void RunTimerUpdate();
        void RunTimerRegister();

        // 查找定时器
        KFTimerData* FindTimerData( const std::string& module, uint64 objectid );

        // 添加定时器
        bool AddTimerData( const std::string& module, uint64 objectid, KFTimerData* kfdata );

        // 删除定时器
        bool RemoveTimerData( const std::string& module );
        bool RemoveTimerData( const std::string& module, uint64 objectid );

        // 从槽里删除定时器
        void RemoveSlotTimer( KFTimerData* kfdata );

        // 添加槽定时器
        void AddSlotTimer( KFTimerData* kfdata, bool firstadd );

        // 刷新定时器
        void RunSlotTimer();

    protected:
        // 当前槽索引
        uint32 _now_slot;

        // 定时器列表
        KFTimerData* _slot_timer_data[ TimerEnum::MaxSlot ];

        // 上一次刷新时间
        uint64 _last_update_time;

        // 定时器数据
        std::map< std::string, std::map< uint64, KFTimerData* > > _kf_timer_data;

        // 需要注册的定时器
        std::list< KFTimerData* > _register_timer_data;
    };

    /////////////////////////////////////////////////////////////////////////////
    static auto _kf_timer_manage = KFTimerManage::Instance();
    /////////////////////////////////////////////////////////////////////////////
}






#endif