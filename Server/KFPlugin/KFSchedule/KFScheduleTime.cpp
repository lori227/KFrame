#include "KFScheduleTime.h"

namespace KFrame
{
    KFScheduleTime::KFScheduleTime()
    {
        _type = Date;
        _year = 0;
        _month = 0;
        _dayofweek = 0;
        _day = 0;
        _hour = 0;
        _minute = 0;
        _control = Loop;
        _data = nullptr;
        _size = 0;
    }

    KFScheduleTime::~KFScheduleTime()
    {
        __KF_FREE__( int8, _data );
    }

    void KFScheduleTime::SetDayOfWeek( uint32 controltype, uint32 minute, uint32 hour, uint32 dayofweek, uint32 month /* = 0 */ )
    {
        _minute = minute;
        _hour = hour;
        _month = month;
        _dayofweek = dayofweek;
        _control = controltype;
        _type = KFScheduleEnum::Week;
    }

    // 设置时间
    void KFScheduleTime::SetTime( uint64 time )
    {
        KFDate kfdate( time );

        _year = kfdate.GetYear();
        _month = kfdate.GetMonth();
        _day = kfdate.GetDay();
        _hour = kfdate.GetHour();
        _minute = kfdate.GetMinute();

        _type = KFScheduleEnum::Date;
        _control = KFScheduleEnum::Once;
    }

    // 设置日期
    void KFScheduleTime::SetDate( uint32 control, uint32 minute, uint32 hour /* = 0 */, uint32 day /* = 0 */, uint32 month /* = 0 */, uint32 year /* = 0 */ )
    {
        _minute = minute;
        _hour = hour;
        _day = day;
        _month = month;
        _year = year;
        _control = control;
        _type = KFScheduleEnum::Date;
    }

    // 设置回调数据
    void KFScheduleTime::SetData( uint64 objectid, const void* data, uint32 size )
    {
        _object_id = objectid;
        if ( data == nullptr || size == 0 )
        {
            return;
        }

        _size = size;
        _data = __KF_MALLOC__( int8, size );
        memcpy( _data, data, size );
    }
}