#ifndef __KF_SCHEDULE_MODULE_H__
#define __KF_SCHEDULE_MODULE_H__


/************************************************************************
//    @Module			:    游戏计划任务模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-13
************************************************************************/

#include "KFrame.h"
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
        void RemoveSchedule( const std::string& module );
    };
}



#endif