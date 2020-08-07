#ifndef __KF_SCHEDULE_DATA_H__
#define __KF_SCHEDULE_DATA_H__

#include "KFrame.h"
#include "KFScheduleInterface.h"

namespace KFrame
{
    namespace KFScheduleEnum
    {
        enum MyEnum
        {
            Stop = 0,	// 停止状态
            Runing = 1,	// 执行状态
        };
    }

    /////////////////////////////////////////////////////////////////////////
    class KFScheduleData
    {
    public:
        // 时间id
        uint32 _time_id = 0u;

        // 模块
        KFModule* _module = nullptr;

        // 对象id
        uint64 _object_id = 0u;

        // 开始回调函数
        KFScheduleFunction _start_function = nullptr;

        // 结束回调函数
        KFScheduleFunction _finish_function = nullptr;
    };
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    class KFScheduleDataList
    {
    public:
        // 时间id
        uint32 _time_id = 0u;

        // 状态
        uint32 _status = KFScheduleEnum::Stop;

        // 持续时间
        uint64 _duration_time = 0u;

        // 保存结束的时间
        KFTimeData _finish_time_data;

        // 计划任务列表
        KFHashMap< KFModule*, KFModule*, KFScheduleData > _schedule_data_list;
    };
}

#endif