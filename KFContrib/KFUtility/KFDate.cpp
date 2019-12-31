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

        _time = static_cast< uint64 >( mktime( &_tm_date ) );
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

    //m_now = chrono::system_clock::now();
    //m_milli = chrono::time_point_cast< chrono::milliseconds >( m_now ).time_since_epoch().count();

    uint64 KFDate::FromString( const std::string& ymd )
    {
        if ( ymd.empty() )
        {
            return 0;
        }

        // 必须按照格式来填写时间
        int32 year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
        sscanf( ymd.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second );

        KFDate kfdate( year, month, day, hour, minute, second );
        return kfdate.GetTime();
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
        KFDate nowtime = KFDate::GetDate();
        return nowtime.GetTime();
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
        time_t temptime = time;
        auto ptm = localtime( &temptime );

        auto year = ptm->tm_year + KFTimeEnum::SinceYear;
        auto month = ptm->tm_mon + 1;
        auto day = ptm->tm_mday;
        auto hour = ptm->tm_hour;
        auto minute = ptm->tm_min;
        auto second = ptm->tm_sec;

        return __FORMAT__( "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}",
                           year, month, day, hour, minute, second );
    }

    std::string KFDate::GetTimeString()
    {
        return GetTimeString( KFDate::GetTimeEx() );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFDate::CheckInTime( uint64 start, uint64 end, uint64 now )
    {
        return now >= start && now <= end;
    }

    bool KFDate::CheckPassTime( uint64 starttime, uint64 keeptime )
    {
        uint64 nowtime = KFDate::GetTimeEx();
        return nowtime >= ( starttime + keeptime );
    }

    bool KFDate::CheckPassTime( uint64 nowtime, uint64 starttime, uint64 keeptime )
    {
        return nowtime >= ( starttime + keeptime );
    }

    uint64 KFDate::GetLeftTime( uint64 nowtime, uint64 starttime, uint32 keeptime )
    {
        if ( starttime > nowtime )
        {
            return 0;
        }

        uint64 passtime = nowtime - starttime;
        if ( passtime >= keeptime )
        {
            return 0;
        }

        return keeptime - passtime;
    }

    bool KFDate::CheckPassHour( uint64 lasttime, uint64 nowtime, uint32 hour )
    {
        if ( hour == 0u )
        {
            hour = 1u;
        }

        auto timedistance = nowtime - lasttime ;
        if ( timedistance >= KFTimeEnum::OneHourSecond * hour )
        {
            return true;
        }

        return false;
    }

    bool KFDate::CheckPassHour( KFDate& lastdate, KFDate& nowdate, uint32 hour )
    {
        return CheckPassHour( lastdate.GetTime(), nowdate.GetTime(), hour );
    }

    bool KFDate::CheckPassMinute( uint64 lasttime, uint64 nowtime, uint32 minute )
    {
        if ( minute == 0u )
        {
            minute = 1u;
        }

        auto timedistance = nowtime - lasttime;
        if ( timedistance >= KFTimeEnum::OneMinuteSecond * minute )
        {
            return true;
        }

        return false;
    }

    bool KFDate::CheckPassMinute( KFDate& lastdate, KFDate& nowdate, uint32 minute )
    {
        return CheckPassMinute( lastdate.GetTime(), nowdate.GetTime(), minute );
    }

    bool KFDate::CheckSameDay( uint64 lasttime, uint64 nowtime )
    {
        return !CheckPassDay( lasttime, nowtime, 0 );
    }

    bool KFDate::CheckSameDay( KFDate& lastdate, KFDate& nowdate )
    {
        return !CheckPassDay( lastdate, nowdate, 0 );
    }

    bool KFDate::CheckPassDay( uint64 lasttime, uint64 nowtime, uint32 hour )
    {
        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );
        return CheckPassDay( lastdate, nowdate, hour );
    }

    bool KFDate::CheckPassDay( KFDate& lastdate, KFDate& nowdate, uint32 hour )
    {
        // 大于一天
        auto timedistance = nowdate.GetTime() - lastdate.GetTime();
        if ( timedistance >= KFTimeEnum::OneDaySecond )
        {
            return true;
        }

        auto nowhour = nowdate.GetHour();
        auto nowday = nowdate.GetDay();

        auto lasthour = lastdate.GetHour();
        auto lastday = lastdate.GetDay();

        // 不同天了
        if ( nowday != lastday )
        {
            return lasthour <= hour || nowhour >= hour;
        }

        return ( lasthour < hour && nowhour >= hour );
    }

    bool KFDate::CheckPassWeek( uint64 lasttime, uint64 nowtime, uint32 dayofweek, uint32 hour )
    {
        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );
        return CheckPassWeek( lastdate, nowdate, dayofweek, hour );
    }

    bool KFDate::CheckPassWeek( KFDate& lastdate, KFDate& nowdate, uint32 dayofweek, uint32 hour )
    {
        // 跨度超过1周
        auto timedistance = nowdate.GetTime() - lastdate.GetTime();
        if ( timedistance >= KFTimeEnum::OneWeekSecond )
        {
            return true;
        }

        auto nowdayofweek = nowdate.GetDayOfWeek();
        auto lastdayofweek = lastdate.GetDayOfWeek();

        auto nowhour = nowdate.GetHour();
        auto lasthour = lastdate.GetHour();

        auto nowweek = nowdate.GetTime() / KFTimeEnum::OneWeekSecond;
        auto lastweek = lastdate.GetTime() / KFTimeEnum::OneWeekSecond;

        // 同一周
        if ( nowweek == lastweek )
        {
            if ( lastdayofweek < dayofweek )
            {
                if ( nowdayofweek > dayofweek )
                {
                    return true;
                }
                else if ( nowdayofweek == dayofweek )
                {
                    if ( nowhour >= hour )
                    {
                        return true;
                    }
                }
            }
            else if ( lastdayofweek == dayofweek )
            {
                if ( lasthour >= hour || nowhour < hour )
                {
                    return false;
                }

                return true;
            }

            return false;
        }

        // 不同周
        if ( nowdayofweek > dayofweek )
        {
            return true;
        }
        else if ( nowdayofweek == dayofweek )
        {
            if ( nowhour >= hour )
            {
                return true;
            }
        }

        return false;
    }

    bool KFDate::CheckPassMonth( uint64 lasttime, uint64 nowtime, uint32 day, uint32 hour )
    {
        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );
        return CheckPassMonth( lastdate, nowdate, day, hour );
    }

    bool KFDate::CheckPassMonth( KFDate& lastdate, KFDate& nowdate, uint32 day, uint32 hour )
    {
        // 间隔超过一个月
        auto monthdistance = ( nowdate.GetYear() - lastdate.GetYear() ) * 12 + nowdate.GetMonth() - lastdate.GetMonth();
        if ( monthdistance >= 2 )
        {
            return true;
        }

        auto nowday = nowdate.GetDay();
        auto lastday = lastdate.GetDay();

        auto nowhour = nowdate.GetHour();
        auto lasthour = lastdate.GetHour();

        // 同一个月
        if ( monthdistance == 0 )
        {
            if ( lastday < day )
            {
                if ( nowday > day )
                {
                    return true;
                }
                else if ( nowday == day )
                {
                    if ( nowhour >= hour )
                    {
                        return true;
                    }
                }
            }
            else if ( lastday == day )
            {
                if ( lasthour >= hour || nowhour < hour )
                {
                    return false;
                }

                return true;
            }

            return false;
        }

        // 不同月
        if ( nowday > day )
        {
            return true;
        }
        else if ( nowday == day )
        {
            if ( nowhour >= hour )
            {
                return true;
            }
        }

        return false;
    }

    bool KFDate::CheckPassYear( uint64 lasttime, uint64 nowtime )
    {
        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );
        return CheckPassYear( lastdate, nowdate );
    }

    bool KFDate::CheckPassYear( KFDate& lastdate, KFDate& nowdate )
    {
        return nowdate.GetYear() != lastdate.GetYear();
    }

    bool KFDate::CheckTime( const KFTimeData* timedata, uint64 lasttime, uint64 nowtime )
    {
        switch ( timedata->_type )
        {
        case KFTimeEnum::Hour:
            return KFDate::CheckPassHour( lasttime, nowtime, timedata->_value );
            break;
        case KFTimeEnum::Minute:	// 分钟
            return KFDate::CheckPassMinute( lasttime, nowtime, timedata->_value );
            break;
        }

        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );
        return CheckTime( timedata, lastdate, nowdate );
    }

    bool KFDate::CheckTime( const KFTimeData* timedata, KFDate& lastdate, KFDate& nowdate )
    {
        switch ( timedata->_type )
        {
        case KFTimeEnum::Hour:
            return KFDate::CheckPassHour( lastdate, nowdate, timedata->_value );
            break;
        case KFTimeEnum::Day:		// 判断时间
            return KFDate::CheckPassDay( lastdate, nowdate, timedata->_hour );
            break;
        case KFTimeEnum::Week:	// 判断时间
            return KFDate::CheckPassWeek( lastdate, nowdate, timedata->_value, timedata->_hour );
            break;
        case KFTimeEnum::Month:	// 月份
            return KFDate::CheckPassMonth( lastdate, nowdate, timedata->_value, timedata->_hour );
            break;
        case KFTimeEnum::Minute:	// 分钟
            return KFDate::CheckPassMinute( lastdate, nowdate, timedata->_value );
            break;
        default:
            break;
        }

        return false;
    }

    bool KFDate::CheckPassTime( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 minute )
    {
        uint64 nowtime = KFDate::GetTimeEx();
        auto ptm = localtime( reinterpret_cast< time_t* >( &nowtime ) );
        uint32 nowyear = ptm->tm_year + KFTimeEnum::SinceYear;
        uint32 nowmonth = ptm->tm_mon + 1;
        uint32 nowday = ptm->tm_mday;
        uint32 nowhour = ptm->tm_hour;
        uint32 nowminute = ptm->tm_min;

        if ( year != 0 )
        {
            if ( nowyear > year )
            {
                return true;
            }
            if ( nowyear < year )
            {
                return false;
            }
        }

        if ( month != 0 )
        {
            if ( nowmonth > month )
            {
                return true;
            }
            if ( nowmonth < month )
            {
                return false;
            }
        }

        if ( day != 0 )
        {
            if ( nowday > day )
            {
                return true;
            }
            if ( nowday < day )
            {
                return false;
            }
        }

        if ( hour != 0 )
        {
            if ( nowhour > hour )
            {
                return true;
            }
            if ( nowhour < hour )
            {
                return false;
            }
        }

        return nowminute >= minute;
    }

    uint64 KFDate::CalcZeroTime( uint64 time, int32 daycount /* = 0 */ )
    {
        KFDate date( ( int64 )time + daycount * KFTimeEnum::OneDaySecond );
        KFDate newdate( date.GetYear(), date.GetMonth(), date.GetDay(), 0, 0 );
        return newdate.GetTime();
    }

    uint64 KFDate::CalcTimeData( const KFTimeData* timedata, uint64 time, int32 count /* = 0 */ )
    {
        uint64 result = time;
        if ( timedata->_type == KFTimeEnum::Hour )
        {
            int64 hour = time / KFTimeEnum::OneHourSecond;
            hour += count;
            result = ( uint64 )hour * KFTimeEnum::OneHourSecond;
        }
        else if ( timedata->_type == KFTimeEnum::Day )
        {
            KFDate date( time );
            if ( date.GetHour() < timedata->_value )
            {
                count -= 1;
            }

            result = CalcZeroTime( time, count ) + timedata->_hour * KFTimeEnum::OneHourSecond;
        }
        else if ( timedata->_type == KFTimeEnum::Week )
        {
            int64 week = time / KFTimeEnum::OneWeekSecond;
            auto daysecond = ( timedata->_value - __MIN__( timedata->_value, 1 ) ) * KFTimeEnum::OneDayHour + timedata->_hour * KFTimeEnum::OneHourSecond;
            if ( time - ( uint64 )week * KFTimeEnum::OneWeekSecond < daysecond )
            {
                week -= 1;
            }

            week += count;
            // 1970/1/1是周4, 所以我们加上4天时间, 把时间从周1作为周期开始
            result = CalcZeroTime( ( uint64 )week * KFTimeEnum::OneWeekSecond + 4 * KFTimeEnum::OneDaySecond ) + daysecond;
        }
        else if ( timedata->_type == KFTimeEnum::Month )
        {
            KFDate date( time );
            auto totalmonth = date.GetYear() * 12 + date.GetMonth() + count;
            if ( date.GetDay() < timedata->_value ||
                    ( date.GetDay() == timedata->_value && date.GetHour() < timedata->_hour ) )
            {
                totalmonth -= 1;
            }

            auto year = totalmonth / 12;
            auto month = totalmonth % 12;
            KFDate newdate( year, month, timedata->_value, timedata->_hour, 0, 0 );
            result = newdate.GetTime();
        }

        return result;
    }
}
