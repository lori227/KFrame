#include "KFScheduleData.h"
#include "KFScheduleTime.h"

namespace KFrame
{
    KFScheduleData::KFScheduleData()
    {
        _last_execute_time = 0;
        _schedule_time = nullptr;
    }

    KFScheduleData::~KFScheduleData()
    {
        __KF_DELETE__( KFScheduleTime, _schedule_time );
    }

    bool KFScheduleData::Execute( uint64 year, uint64 month, uint64 day, uint64 dayofweek, uint64 hour, uint64 minute )
    {
        if ( _schedule_time->_minute != minute )
        {
            return false;
        }

        if ( _schedule_time->_hour != 0 && _schedule_time->_hour != hour )
        {
            return false;
        }

        // 判断时间
        switch ( _schedule_time->_type )
        {
        case KFScheduleEnum::Date:
        {
            if ( _schedule_time->_day != 0 && _schedule_time->_day != day )
            {
                return false;
            }

            if ( _schedule_time->_month != 0 && _schedule_time->_month != month )
            {
                return false;
            }

            if ( _schedule_time->_year != 0 && _schedule_time->_year != year )
            {
                return false;
            }

            break;
        }
        case KFScheduleEnum::Week:
        {
            if ( _schedule_time->_dayofweek != 0 && _schedule_time->_dayofweek != dayofweek )
            {
                return false;
            }

            if ( _schedule_time->_month != 0 && _schedule_time->_month != month )
            {
                return false;
            }

            break;
        }
        default:
            return true;
            break;

        }

        // 不超过一分钟
        auto gametime = KFGlobal::Instance()->_game_time;
        if ( gametime < ( _last_execute_time + KFTimeEnum::OneMinuteMicSecond + 1 ) )
        {
            return false;
        }

        // 执行任务, 设置执行时间
        _last_execute_time = gametime;
        _function( _schedule_time->_object_id, _schedule_time->_data, _schedule_time->_size );
        return _schedule_time->_control == Once;
    }
}