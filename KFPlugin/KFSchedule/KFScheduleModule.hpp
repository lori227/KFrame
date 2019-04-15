#ifndef __KF_SCHEDULE_MODULE_H__
#define __KF_SCHEDULE_MODULE_H__


/************************************************************************
//    @Module			:    游戏计划任务模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-13
************************************************************************/

#include "KFScheduleData.hpp"
#include "KFScheduleTime.hpp"
#include "KFScheduleInterface.h"

namespace KFrame
{
    class KFScheduleModule : public KFScheduleInterface
    {
    public:
        KFScheduleModule();
        ~KFScheduleModule();

        virtual void Run();

        // 逻辑
        ////////////////////////////////////////////////////////////////////////////////
        // 创建计划设定
        virtual KFScheduleSetting* CreateScheduleSetting();

    protected:
        // 注册计划任务
        virtual void AddSchedule( const std::string& module, KFScheduleSetting* kfsetting, KFScheduleFunction& function );

        // 删除计划任务
        virtual void RemoveSchedule( const std::string& module );
        virtual void RemoveSchedule( const std::string& module, uint64 objectid );

    protected:
        // 添加计划任务
        void AddSchedule( KFScheduleData* kfdata );

        // 删除计划任务
        void RemoveScheduleData( const std::string& module, uint64 objectid );

        void RunScheduleUpdate();
        void RunScheduleRegister();
        void RunScheduleRemove();

    private:
        // 需要注册的计划任务
        std::list< KFScheduleData* > _kf_schedule_register;

        // 计划任务
        std::map< std::string, std::map< uint64, KFScheduleData* > > _kf_schedule_data;

        // 需要删除的任务
        std::list< std::tuple< std::string, uint64 > > _kf_schedule_remove;

        // 更新定时器
        KFClockTimer _update_timer;
    };
}



#endif