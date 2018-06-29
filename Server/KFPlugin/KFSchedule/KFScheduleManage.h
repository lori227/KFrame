#ifndef __KF_SCHEDULE_MANAGE_H__
#define __KF_SCHEDULE_MANAGE_H__

#include "KFScheduleData.h"
#include "KFTime/KFTimer.h"
#include "KFScheduleInterface.h"

// 非线程安全
namespace KFrame
{
    class KFScheduleManage : public KFSingleton< KFScheduleManage >
    {
    public:
        KFScheduleManage();
        ~KFScheduleManage();

        // 创建计划设定
        KFScheduleSetting* CreateScheduleSetting();

        // 注册计划任务
        void RegisterSchedule( const std::string& module, KFScheduleSetting* kfsetting, KFScheduleFunction& function );

        // 删除计划任务
        void UnRegisterSchedule( const std::string& module );

        // 更新
        void RunUpdate();
    protected:
        // 添加计划任务
        void AddSchedule( KFScheduleData* kfdata );

        // 删除计划任务
        void AddRemoveSchedule( const std::string& module, uint32 objectid );
        void RemoveSchedule( const std::string& module, uint32 objectid );

        void RunScheduleUpdate();
        void RunScheduleRegister();
        void RunScheduleRemove();

    protected:

        // 需要注册的计划任务
        std::list< KFScheduleData* > _kf_schedule_register;

        // 计划任务
        std::map< std::string, std::map< uint32, KFScheduleData* > > _kf_schedule_data;

        // 需要删除的任务
        std::map< std::string, uint32 > _kf_schedule_remove;

        // 更新定时器
        KFClockTimer _update_timer;
    };

    /////////////////////////////////////////////////////////////////////////////
    static auto _kf_schedule_manage = KFScheduleManage::Instance();
    /////////////////////////////////////////////////////////////////////////////
}

#endif