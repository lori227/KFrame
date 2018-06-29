#ifndef __KF_SCHEDULE_TIME_H__
#define __KF_SCHEDULE_TIME_H__

#include "KFrame.h"
#include "KFScheduleInterface.h"

namespace KFrame
{
    // 计划时间
    class KFScheduleTime : public KFScheduleSetting
    {
    public:
        KFScheduleTime();
        ~KFScheduleTime();

        // 设置星期
        virtual void SetDayOfWeek( uint32 controltype, uint32 minute, uint32 hour, uint32 dayofweek, uint32 month = 0 );

        // 设置时间
        virtual void SetTime( uint64 time );

        // 设置日期
        virtual void SetDate( uint32 control, uint32 minute, uint32 hour, uint32 day = 0, uint32 month = 0, uint32 year = 0 );

        // 设置回调数据
        virtual void SetData( uint32 objectid, const void* data, uint32 size );

    public:
        uint32 _type;		// 类型
        uint32 _year;		// 年
        uint32 _month;		// 月
        uint32 _day;		// 日
        uint32 _dayofweek;	// 星期
        uint32 _hour;		// 小时
        uint32 _minute;		// 分钟
        uint32 _control;	// 控制类型

        // 回调参数
        uint32 _object_id;
        int8* _data;
        uint32 _size;
    };
}






#endif