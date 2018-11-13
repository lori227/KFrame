#ifndef __KF_SCHEDULE_DATA_H__
#define __KF_SCHEDULE_DATA_H__

#include "KFrame.h"
#include "KFScheduleInterface.h"

namespace KFrame
{
    class KFScheduleTime;
    class KFScheduleData
    {
    public:
        KFScheduleData();
        ~KFScheduleData();

        // 执行
        bool Execute( uint64 year, uint64 month, uint64 day, uint64 dayofweek, uint64 hour, uint64 minute );

    public:
        // 模块名
        std::string _module;

        // 定时器
        KFScheduleTime* _schedule_time;

        // 类型
        uint64 _last_execute_time;

        // 回调函数
        KFScheduleFunction _function;
    };
}

#endif