#include "KFDate.h"
#include <stdio.h>
#include <time.h>
#include "KFMacros.h"

namespace KFrame
{
    KFDate::KFDate()
    {
        _time = 0;
    }

    KFDate::KFDate( uint64 time )
    {
        _time = time;
        ConvertTimeDate();
    }

    KFDate::KFDate( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 min, uint32 second /* = 0 */ )
    {
        _tm_date.tm_year = static_cast< int32 >( year - KFTimeEnum::SinceYear );     // tm_year is 1970 based
        _tm_date.tm_mon = static_cast< int32 >( month - 1 );        // tm_mon is 0 based
        _tm_date.tm_mday = static_cast< int32 >( day );
        _tm_date.tm_hour = static_cast< int32 >( hour );
        _tm_date.tm_min = static_cast< int32 >( min );
        _tm_date.tm_sec = static_cast< int32 >( second );
        _tm_date.tm_isdst = -1;

        _time = static_cast<uint64>( mktime( &_tm_date ) );
        ConvertTimeDate();
    }

    void KFDate::ConvertTimeDate()
    {
        _tm_date = *localtime( &_time );
        _tm_date.tm_year += KFTimeEnum::SinceYear;
        _tm_date.tm_mon += 1;
        if ( _tm_date.tm_wday == 0 )
        {
            _tm_date.tm_wday = 7;
        }
    }

    uint64 KFDate::FromString( const std::string& ymd )
    {
        if ( ymd.empty() )
        {
            return 0;
        }

        // 必须按照格式来填写时间
        int32 year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
        sscanf( ymd.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second );

        KFDate date( year, month, day, hour, minute, second );
        return date.GetTime();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    KFDate KFDate::GetDate()
    {
#ifdef _WIN32
        return KFDate( _time32( nullptr ) );
#else
        return KFDate( time( 0 ) );
#endif
    }

    uint64 KFDate::GetTime() const
    {
        return _time;
    }

    void KFDate::SetTime( uint64 time )
    {
        _time = time;
        ConvertTimeDate();
    }

    void KFDate::AddTime( uint64 time )
    {
        _time += time;
        ConvertTimeDate();
    }

    uint64 KFDate::GetTimeEx()
    {
        KFDate now_time = KFDate::GetDate();
        return now_time.GetTime();
    }

    uint32 KFDate::GetYear() const
    {
        return _tm_date.tm_year;
    }

    uint32 KFDate::GetMonth() const
    {
        return _tm_date.tm_mon;
    }

    uint32 KFDate::GetDay() const
    {
        return _tm_date.tm_mday;
    }

    uint32 KFDate::GetHour() const
    {
        return _tm_date.tm_hour;
    }

    uint32 KFDate::GetMinute() const
    {
        return _tm_date.tm_min;
    }

    uint32 KFDate::GetSecond() const
    {
        return _tm_date.tm_sec;
    }

    uint32 KFDate::GetDayOfWeek() const
    {
        return _tm_date.tm_wday;
    }

    uint32 KFDate::GetDayOfYear() const
    {
        return _tm_date.tm_yday;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFDate::GetTimeString( uint64 time )
    {
        time_t temp_time = time;
        auto ptm = localtime( &temp_time );

        auto year = ptm->tm_year + KFTimeEnum::SinceYear;
        auto month = ptm->tm_mon + 1;
        auto day = ptm->tm_mday;
        auto hour = ptm->tm_hour;
        auto minute = ptm->tm_min;
        auto second = ptm->tm_sec;

        return __FORMAT__( "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}", year, month, day, hour, minute, second );
    }

    std::string KFDate::GetTimeString()
    {
        return GetTimeString( KFDate::GetTimeEx() );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckInTime( uint64 start_time, uint64 end_time, uint64 now_time )
    {
        if ( start_time == 0u || end_time == 0u )
        {
            return false;
        }

        return now_time >= start_time && now_time <= end_time;
    }

    bool KFDate::CheckTimeout( uint64 start_time, uint64 keep_time )
    {
        auto now_time = KFDate::GetTimeEx();
        return now_time >= ( start_time + keep_time );
    }

    bool KFDate::CheckTimeout( uint64 now_time, uint64 start_time, uint64 keep_time )
    {
        return now_time >= ( start_time + keep_time );
    }

    uint64 KFDate::CalcLeftTime( uint64 now_time, uint64 start_time, uint32 keep_time )
    {
        if ( start_time > now_time )
        {
            return 0;
        }

        auto pass_time = now_time - start_time;
        if ( pass_time >= keep_time )
        {
            return 0;
        }

        return keep_time - pass_time;
    }

    bool KFDate::CheckSameDay( uint64 last_time, uint64 now_time )
    {
        KFDate now_date( now_time );
        KFDate last_date( last_time );
        return CheckSameDay( last_date, now_date );
    }

    bool KFDate::CheckSameDay( KFDate& last_date, KFDate& now_date )
    {
        // 相差时间超过1天
        if ( now_date.GetTime() >= last_date.GetTime() + KFTimeEnum::OneDaySecond )
        {
            return false;
        }

        // 判断日期相同
        return now_date.GetDay() == last_date.GetDay();
    }

    bool KFDate::CheckPassTime( uint64 now_time, uint64 next_time )
    {
        return now_time >= next_time;
    }

    bool KFDate::CheckPassTime( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 minute )
    {
        auto now_date = KFDate::GetDate();

        year = ( year != 0 ? year : now_date.GetYear() );
        month = ( month != 0 ? month : now_date.GetMonth() );
        day = ( day != 0 ? day : now_date.GetDay() );

        KFDate checkdate( year, month, day, hour, minute );
        return now_date.GetTime() >= checkdate.GetTime();
    }

    uint64 KFDate::CalcZeroTime( uint64 time, int32 day_count /* = 0 */ )
    {
        KFDate date( ( int64 )time + day_count * KFTimeEnum::OneDaySecond );
        KFDate new_date( date.GetYear(), date.GetMonth(), date.GetDay(), 0, 0 );
        return new_date.GetTime();
    }

    uint64 KFDate::CalcTimeData( const KFTimeData* time_data, uint64 time, int32 count /* = 0 */ )
    {
        auto result = time;
        switch ( time_data->_flag )
        {
        case KFTimeEnum::Minute:
        {
            int64 minute = time / KFTimeEnum::OneMinuteSecond;
            minute += count * time_data->_minute;
            result = ( uint64 )minute * KFTimeEnum::OneMinuteSecond;
        }
        break;
        case KFTimeEnum::Hour:
        {
            int64 hour = time / KFTimeEnum::OneHourSecond;
            hour += count * time_data->_hour;
            result = ( uint64 )hour * KFTimeEnum::OneHourSecond;
        }
        break;
        case KFTimeEnum::Day:
        {
            KFDate date( time );
            auto date_time = date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
            auto check_time = time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
            if ( date_time < check_time )
            {
                count -= 1;
            }

            // 计算0点时间
            result = CalcZeroTime( time, count );
            result += time_data->_hour * KFTimeEnum::OneHourSecond + time_data->_minute * KFTimeEnum::OneMinuteSecond;
        }
        break;
        case KFTimeEnum::Week:
        {
            int64 week = time / KFTimeEnum::OneWeekSecond;
            auto day_second = ( time_data->_day_of_week - __MIN__( time_data->_day_of_week, 1 ) ) * KFTimeEnum::OneDaySecond;
            day_second += time_data->_hour * KFTimeEnum::OneHourSecond + time_data->_minute * KFTimeEnum::OneMinuteSecond;
            if ( time - ( uint64 )week * KFTimeEnum::OneWeekSecond < day_second )
            {
                week -= 1;
            }

            week += count;
            // 1970/1/1是周4, 所以我们加上4天时间, 把时间从周1作为周期开始
            result = CalcZeroTime( ( uint64 )week * KFTimeEnum::OneWeekSecond + 4 * KFTimeEnum::OneDaySecond ) + day_second;
        }
        break;
        case KFTimeEnum::Month:
        {
            KFDate date( time );
            auto date_time = date.GetDay() * KFTimeEnum::OneDayMinute + date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
            auto check_time = time_data->_day * KFTimeEnum::OneDayMinute + time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
            if ( date_time < check_time )
            {
                count -= 1;
            }

            auto total_month = date.GetYear() * 12 + date.GetMonth() + count;
            auto year = total_month / 12;
            auto month = total_month % 12;
            KFDate new_date( year, month, time_data->_day, time_data->_hour, time_data->_minute );
            result = new_date.GetTime();
        }
        break;
        default:
            break;
        }

        return result;
    }

    bool KFDate::CheckLoopTimeData( const KFTimeData* time_data, uint64 last_time, uint64 now_time )
    {
        switch ( time_data->_flag )
        {
        case KFTimeEnum::Minute:	// 分钟
            return CheckLoopMinute( time_data, last_time, now_time );
            break;
        case KFTimeEnum::Hour:		// 小时
            return CheckLoopHour( time_data, last_time, now_time );
            break;
        case KFTimeEnum::Ever:
            return CheckLoopEver( last_time );
            break;
        }

        KFDate now_date( now_time );
        KFDate last_date( last_time );
        return CheckLoopTimeData( time_data, last_date, now_date );
    }

    bool KFDate::CheckLoopTimeData( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date )
    {
        switch ( time_data->_flag )
        {
        case KFTimeEnum::Minute:	// 分钟
            return CheckLoopMinute( time_data, last_date, now_date );
            break;
        case KFTimeEnum::Hour:		// 小时
            return CheckLoopHour( time_data, last_date, now_date );
            break;
        case KFTimeEnum::Day:		// 每天
            return CheckLoopDay( time_data, last_date, now_date );
            break;
        case KFTimeEnum::Week:		// 星期
            return CheckLoopWeek( time_data, last_date, now_date );
            break;
        case KFTimeEnum::Month:		// 月份
            return CheckLoopMonth( time_data, last_date, now_date );
            break;
        case KFTimeEnum::Ever:		// 永久
            return CheckLoopEver( last_date );
        default:
            break;
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopMinute( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date )
    {
        return CheckLoopMinute( time_data, last_date.GetTime(), now_date.GetTime() );
    }

    bool KFDate::CheckLoopMinute( const KFTimeData* time_data, uint64 last_time, uint64 now_time )
    {
        auto second = time_data->_minute * KFTimeEnum::OneMinuteSecond;
        return now_time >= ( last_time + second );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopHour( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date )
    {
        return CheckLoopHour( time_data, last_date.GetTime(), now_date.GetTime() );
    }

    bool KFDate::CheckLoopHour( const KFTimeData* time_data, uint64 last_time, uint64 now_time )
    {
        auto second = time_data->_hour * KFTimeEnum::OneHourSecond;
        return now_time >= ( last_time + second );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopDay( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date )
    {
        // 间隔时间大于一天
        auto time_distance = now_date.GetTime() - last_date.GetTime();
        if ( time_distance >= KFTimeEnum::OneDaySecond )
        {
            return true;
        }

        auto check_minute = time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
        auto now_minute = now_date.GetHour() * KFTimeEnum::OneHourMinute + now_date.GetMinute();
        auto last_minute = last_date.GetHour() * KFTimeEnum::OneHourMinute + last_date.GetMinute();

        auto now_day = now_date.GetDay();
        auto last_day = last_date.GetDay();

        // 不同天了
        if ( now_day != last_day )
        {
            return last_minute < check_minute || now_minute >= check_minute;
        }

        // 同一天
        return ( last_minute < check_minute && now_minute >= check_minute );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopWeek( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date )
    {
        // 跨度超过1周
        auto time_distance = now_date.GetTime() - last_date.GetTime();
        if ( time_distance >= KFTimeEnum::OneWeekSecond )
        {
            return true;
        }

        auto check_minute = time_data->_day_of_week * KFTimeEnum::OneDayMinute + time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
        auto now_minute = now_date.GetDayOfWeek() * KFTimeEnum::OneDayMinute + now_date.GetHour() * KFTimeEnum::OneHourMinute + now_date.GetMinute();
        auto last_minute = last_date.GetDayOfWeek() * KFTimeEnum::OneDayMinute + last_date.GetHour() * KFTimeEnum::OneHourMinute + last_date.GetMinute();

        auto now_week = now_date.GetTime() / KFTimeEnum::OneWeekSecond;
        auto last_week = last_date.GetTime() / KFTimeEnum::OneWeekSecond;

        // 不同周
        if ( now_week != last_week )
        {
            return last_minute < check_minute || now_minute >= check_minute;
        }

        // 不同周
        return ( last_minute < check_minute && now_minute >= check_minute );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopMonth( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date )
    {
        // 间隔超过一个月
        auto month_distance = ( now_date.GetYear() - last_date.GetYear() ) * 12 + now_date.GetMonth() - last_date.GetMonth();
        if ( month_distance > 1u )
        {
            return true;
        }

        auto check_minute = time_data->_day * KFTimeEnum::OneDayMinute + time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
        auto now_minute = now_date.GetDay() * KFTimeEnum::OneDayMinute + now_date.GetHour() * KFTimeEnum::OneHourMinute + now_date.GetMinute();
        auto last_minute = last_date.GetDay() * KFTimeEnum::OneDayMinute + last_date.GetHour() * KFTimeEnum::OneHourMinute + last_date.GetMinute();

        // 不同月
        if ( month_distance == 1u )
        {
            return last_minute < check_minute || now_minute >= check_minute;
        }

        // 相同月
        return ( last_minute < check_minute && now_minute >= check_minute );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopYear( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date )
    {
        // 间隔超过一年
        auto year_distance = ( now_date.GetYear() - last_date.GetYear() );
        if ( year_distance > 1u )
        {
            return true;
        }

        KFDate check_date( now_date.GetYear(), time_data->_month, time_data->_day, time_data->_hour, time_data->_minute );
        return ( last_date.GetTime() < check_date.GetTime() && now_date.GetTime() >= check_date.GetTime() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopEver( uint64 last_time )
    {
        return last_time == 0u;
    }

    bool KFDate::CheckLoopEver( KFDate& last_date )
    {
        return last_date.GetTime() == 0u;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckInTimeSection( const KFTimeSection* time_section, uint64 time )
    {
        KFDate date( time );
        return CheckInTimeSection( time_section, date );
    }

    bool KFDate::CheckInTimeSection( const KFTimeSection* time_section, KFDate& date )
    {
        return CheckSectionTimeData( &time_section->_start_time_data, date ) &&
               !CheckSectionTimeData( &time_section->_finish_time_data, date );
    }

    bool KFDate::CheckSectionTimeData( const KFTimeData* time_data, KFDate& date )
    {
        switch ( time_data->_flag )
        {
        case KFTimeEnum::Minute:	// 分钟
            return CheckSectionMinute( time_data, date );
            break;
        case KFTimeEnum::Hour:		// 小时
            return CheckSectionHour( time_data, date );
            break;
        case KFTimeEnum::Day:		// 每天
            return CheckSectionDay( time_data, date );
            break;
        case KFTimeEnum::Week:		// 星期
            return CheckSectionWeek( time_data, date );
            break;
        case KFTimeEnum::Month:		// 月份
            return CheckSectionMonth( time_data, date );
            break;
        case KFTimeEnum::Year:		// 年份
            return CheckSectionYear( time_data, date );
            break;
        default:
            break;
        }

        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionMinute( const KFTimeData* time_data, KFDate& date )
    {
        return date.GetMinute() > time_data->_minute;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionHour( const KFTimeData* time_data, KFDate& date )
    {
        auto date_time = date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
        auto check_time = time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
        return date_time >= check_time;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionDay( const KFTimeData* time_data, KFDate& date )
    {
        auto date_time = date.GetDay() * KFTimeEnum::OneDayMinute + date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
        auto check_time = time_data->_day * KFTimeEnum::OneDayMinute +  time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
        return date_time >= check_time;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionWeek( const KFTimeData* time_data, KFDate& date )
    {
        auto date_time = date.GetDayOfWeek() * KFTimeEnum::OneDayMinute + date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
        auto check_time = time_data->_day_of_week * KFTimeEnum::OneDayMinute + time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
        return date_time >= check_time;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionMonth( const KFTimeData* time_data, KFDate& date )
    {
        auto date_time = ( date.GetMonth() * KFTimeEnum::OneMonthDay +  date.GetDay() ) * KFTimeEnum::OneDayMinute + date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
        auto check_time = ( time_data->_month * KFTimeEnum::OneMonthDay + time_data->_day ) * KFTimeEnum::OneDayMinute + time_data->_hour * KFTimeEnum::OneHourMinute + time_data->_minute;
        return date_time >= check_time;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionYear( const KFTimeData* time_data, KFDate& date )
    {
        KFDate check_date( time_data->_year, time_data->_month, time_data->_day, time_data->_hour, time_data->_month );
        return date.GetTime() >= check_date.GetTime();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
