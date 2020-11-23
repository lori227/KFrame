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
    bool KFDate::CheckInTime( uint64 starttime, uint64 endtime, uint64 nowtime )
    {
        if ( starttime == 0u || endtime == 0u )
        {
            return false;
        }

        return nowtime >= starttime && nowtime <= endtime;
    }

    bool KFDate::CheckTimeout( uint64 starttime, uint64 keeptime )
    {
        auto nowtime = KFDate::GetTimeEx();
        return nowtime >= ( starttime + keeptime );
    }

    bool KFDate::CheckTimeout( uint64 nowtime, uint64 starttime, uint64 keeptime )
    {
        return nowtime >= ( starttime + keeptime );
    }

    uint64 KFDate::CalcLeftTime( uint64 nowtime, uint64 starttime, uint32 keeptime )
    {
        if ( starttime > nowtime )
        {
            return 0;
        }

        auto passtime = nowtime - starttime;
        if ( passtime >= keeptime )
        {
            return 0;
        }

        return keeptime - passtime;
    }

    bool KFDate::CheckSameDay( uint64 lasttime, uint64 nowtime )
    {
        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );
        return CheckSameDay( lastdate, nowdate );
    }

    bool KFDate::CheckSameDay( KFDate& lastdate, KFDate& nowdate )
    {
        // 相差时间超过1天
        if ( nowdate.GetTime() >= lastdate.GetTime() + KFTimeEnum::OneDaySecond )
        {
            return false;
        }

        // 判断日期相同
        return nowdate.GetDay() == lastdate.GetDay();
    }

    bool KFDate::CheckPassTime( uint64 nowtime, uint64 nexttime )
    {
        return nowtime >= nexttime;
    }

    bool KFDate::CheckPassTime( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 minute )
    {
        auto nowdate = KFDate::GetDate();

        year = ( year != 0 ? year : nowdate.GetYear() );
        month = ( month != 0 ? month : nowdate.GetMonth() );
        day = ( day != 0 ? day : nowdate.GetDay() );

        KFDate checkdate( year, month, day, hour, minute );
        return nowdate.GetTime() >= checkdate.GetTime();
    }

    uint64 KFDate::CalcZeroTime( uint64 time, int32 daycount /* = 0 */ )
    {
        KFDate date( ( int64 )time + daycount * KFTimeEnum::OneDaySecond );
        KFDate newdate( date.GetYear(), date.GetMonth(), date.GetDay(), 0, 0 );
        return newdate.GetTime();
    }

    uint64 KFDate::CalcTimeData( const KFTimeData* timedata, uint64 time, int32 count /* = 0 */ )
    {
        auto result = time;
        switch ( timedata->_flag )
        {
        case KFTimeEnum::Minute:
        {
            int64 minute = time / KFTimeEnum::OneMinuteSecond;
            minute += count * timedata->_minute;
            result = ( uint64 )minute * KFTimeEnum::OneMinuteSecond;
        }
        break;
        case KFTimeEnum::Hour:
        {
            int64 hour = time / KFTimeEnum::OneHourSecond;
            hour += count * timedata->_hour;
            result = ( uint64 )hour * KFTimeEnum::OneHourSecond;
        }
        break;
        case KFTimeEnum::Day:
        {
            KFDate date( time );
            auto datetime = date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
            auto checktime = timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
            if ( datetime < checktime )
            {
                count -= 1;
            }

            // 计算0点时间
            result = CalcZeroTime( time, count );
            result += timedata->_hour * KFTimeEnum::OneHourSecond + timedata->_minute * KFTimeEnum::OneMinuteSecond;
        }
        break;
        case KFTimeEnum::Week:
        {
            int64 week = time / KFTimeEnum::OneWeekSecond;
            auto daysecond = ( timedata->_day_of_week - __MIN__( timedata->_day_of_week, 1 ) ) * KFTimeEnum::OneDaySecond;
            daysecond += timedata->_hour * KFTimeEnum::OneHourSecond + timedata->_minute * KFTimeEnum::OneMinuteSecond;
            if ( time - ( uint64 )week * KFTimeEnum::OneWeekSecond < daysecond )
            {
                week -= 1;
            }

            week += count;
            // 1970/1/1是周4, 所以我们加上4天时间, 把时间从周1作为周期开始
            result = CalcZeroTime( ( uint64 )week * KFTimeEnum::OneWeekSecond + 4 * KFTimeEnum::OneDaySecond ) + daysecond;
        }
        break;
        case KFTimeEnum::Month:
        {
            KFDate date( time );
            auto datetime = date.GetDay() * KFTimeEnum::OneDayMinute + date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
            auto checktime = timedata->_day * KFTimeEnum::OneDayMinute + timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
            if ( datetime < checktime )
            {
                count -= 1;
            }

            auto totalmonth = date.GetYear() * 12 + date.GetMonth() + count;
            auto year = totalmonth / 12;
            auto month = totalmonth % 12;
            KFDate newdate( year, month, timedata->_day, timedata->_hour, timedata->_minute );
            result = newdate.GetTime();
        }
        break;
        default:
            break;
        }

        return result;
    }

    bool KFDate::CheckLoopTimeData( const KFTimeData* timedata, uint64 lasttime, uint64 nowtime )
    {
        switch ( timedata->_flag )
        {
        case KFTimeEnum::Minute:	// 分钟
            return CheckLoopMinute( timedata, lasttime, nowtime );
            break;
        case KFTimeEnum::Hour:		// 小时
            return CheckLoopHour( timedata, lasttime, nowtime );
            break;
        case KFTimeEnum::Ever:
            return CheckLoopEver( lasttime );
            break;
        }

        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );
        return CheckLoopTimeData( timedata, lastdate, nowdate );
    }

    bool KFDate::CheckLoopTimeData( const KFTimeData* timedata, KFDate& lastdate, KFDate& nowdate )
    {
        switch ( timedata->_flag )
        {
        case KFTimeEnum::Minute:	// 分钟
            return CheckLoopMinute( timedata, lastdate, nowdate );
            break;
        case KFTimeEnum::Hour:		// 小时
            return CheckLoopHour( timedata, lastdate, nowdate );
            break;
        case KFTimeEnum::Day:		// 每天
            return CheckLoopDay( timedata, lastdate, nowdate );
            break;
        case KFTimeEnum::Week:		// 星期
            return CheckLoopWeek( timedata, lastdate, nowdate );
            break;
        case KFTimeEnum::Month:		// 月份
            return CheckLoopMonth( timedata, lastdate, nowdate );
            break;
        case KFTimeEnum::Ever:		// 永久
            return CheckLoopEver( lastdate );
        default:
            break;
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopMinute( const KFTimeData* timedata, KFDate& lastdate, KFDate& nowdate )
    {
        return CheckLoopMinute( timedata, lastdate.GetTime(), nowdate.GetTime() );
    }

    bool KFDate::CheckLoopMinute( const KFTimeData* timedata, uint64 lasttime, uint64 nowtime )
    {
        auto second = timedata->_minute * KFTimeEnum::OneMinuteSecond;
        return nowtime >= ( lasttime + second );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopHour( const KFTimeData* timedata, KFDate& lastdate, KFDate& nowdate )
    {
        return CheckLoopHour( timedata, lastdate.GetTime(), nowdate.GetTime() );
    }

    bool KFDate::CheckLoopHour( const KFTimeData* timedata, uint64 lasttime, uint64 nowtime )
    {
        auto second = timedata->_hour * KFTimeEnum::OneHourSecond;
        return nowtime >= ( lasttime + second );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopDay( const KFTimeData* timedata, KFDate& lastdate, KFDate& nowdate )
    {
        // 间隔时间大于一天
        auto timedistance = nowdate.GetTime() - lastdate.GetTime();
        if ( timedistance >= KFTimeEnum::OneDaySecond )
        {
            return true;
        }

        auto checkminute = timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
        auto nowminute = nowdate.GetHour() * KFTimeEnum::OneHourMinute + nowdate.GetMinute();
        auto lastminute = lastdate.GetHour() * KFTimeEnum::OneHourMinute + lastdate.GetMinute();

        auto nowday = nowdate.GetDay();
        auto lastday = lastdate.GetDay();

        // 不同天了
        if ( nowday != lastday )
        {
            return lastminute < checkminute || nowminute >= checkminute;
        }

        // 同一天
        return ( lastminute < checkminute && nowminute >= checkminute );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopWeek( const KFTimeData* timedata, KFDate& lastdate, KFDate& nowdate )
    {
        // 跨度超过1周
        auto timedistance = nowdate.GetTime() - lastdate.GetTime();
        if ( timedistance >= KFTimeEnum::OneWeekSecond )
        {
            return true;
        }

        auto checkminute = timedata->_day_of_week * KFTimeEnum::OneDayMinute + timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
        auto nowminute = nowdate.GetDayOfWeek() * KFTimeEnum::OneDayMinute + nowdate.GetHour() * KFTimeEnum::OneHourMinute + nowdate.GetMinute();
        auto lastminute = lastdate.GetDayOfWeek() * KFTimeEnum::OneDayMinute + lastdate.GetHour() * KFTimeEnum::OneHourMinute + lastdate.GetMinute();

        auto nowweek = nowdate.GetTime() / KFTimeEnum::OneWeekSecond;
        auto lastweek = lastdate.GetTime() / KFTimeEnum::OneWeekSecond;

        // 不同周
        if ( nowweek != lastweek )
        {
            return lastminute < checkminute || nowminute >= checkminute;
        }

        // 不同周
        return ( lastminute < checkminute && nowminute >= checkminute );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopMonth( const KFTimeData* timedata, KFDate& lastdate, KFDate& nowdate )
    {
        // 间隔超过一个月
        auto monthdistance = ( nowdate.GetYear() - lastdate.GetYear() ) * 12 + nowdate.GetMonth() - lastdate.GetMonth();
        if ( monthdistance > 1u )
        {
            return true;
        }

        auto checkminute = timedata->_day * KFTimeEnum::OneDayMinute + timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
        auto nowminute = nowdate.GetDay() * KFTimeEnum::OneDayMinute + nowdate.GetHour() * KFTimeEnum::OneHourMinute + nowdate.GetMinute();
        auto lastminute = lastdate.GetDay() * KFTimeEnum::OneDayMinute + lastdate.GetHour() * KFTimeEnum::OneHourMinute + lastdate.GetMinute();

        // 不同月
        if ( monthdistance == 1u )
        {
            return lastminute < checkminute || nowminute >= checkminute;
        }

        // 相同月
        return ( lastminute < checkminute && nowminute >= checkminute );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopYear( const KFTimeData* timedata, KFDate& lastdate, KFDate& nowdate )
    {
        // 间隔超过一年
        auto yeardistance = ( nowdate.GetYear() - lastdate.GetYear() );
        if ( yeardistance > 1u )
        {
            return true;
        }

        KFDate checkdate( nowdate.GetYear(), timedata->_month, timedata->_day, timedata->_hour, timedata->_minute );
        return ( lastdate.GetTime() < checkdate.GetTime() && nowdate.GetTime() >= checkdate.GetTime() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckLoopEver( uint64 lasttime )
    {
        return lasttime == 0u;
    }

    bool KFDate::CheckLoopEver( KFDate& lastdate )
    {
        return lastdate.GetTime() == 0u;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckInTimeSection( const KFTimeSection* timesection, uint64 time )
    {
        KFDate date( time );
        return CheckInTimeSection( timesection, date );
    }

    bool KFDate::CheckInTimeSection( const KFTimeSection* timesection, KFDate& date )
    {
        return CheckSectionTimeData( &timesection->_start_time_data, date ) &&
               !CheckSectionTimeData( &timesection->_finish_time_data, date );
    }

    bool KFDate::CheckSectionTimeData( const KFTimeData* timedata, KFDate& date )
    {
        switch ( timedata->_flag )
        {
        case KFTimeEnum::Minute:	// 分钟
            return CheckSectionMinute( timedata, date );
            break;
        case KFTimeEnum::Hour:		// 小时
            return CheckSectionHour( timedata, date );
            break;
        case KFTimeEnum::Day:		// 每天
            return CheckSectionDay( timedata, date );
            break;
        case KFTimeEnum::Week:		// 星期
            return CheckSectionWeek( timedata, date );
            break;
        case KFTimeEnum::Month:		// 月份
            return CheckSectionMonth( timedata, date );
            break;
        case KFTimeEnum::Year:		// 年份
            return CheckSectionYear( timedata, date );
            break;
        default:
            break;
        }

        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionMinute( const KFTimeData* timedata, KFDate& date )
    {
        return date.GetMinute() > timedata->_minute;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionHour( const KFTimeData* timedata, KFDate& date )
    {
        auto datetime = date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
        auto checktime = timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
        return datetime >= checktime;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionDay( const KFTimeData* timedata, KFDate& date )
    {
        auto datetime = date.GetDay() * KFTimeEnum::OneDayMinute + date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
        auto checktime = timedata->_day * KFTimeEnum::OneDayMinute +  timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
        return datetime >= checktime;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionWeek( const KFTimeData* timedata, KFDate& date )
    {
        auto datetime = date.GetDayOfWeek() * KFTimeEnum::OneDayMinute + date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
        auto checktime = timedata->_day_of_week * KFTimeEnum::OneDayMinute + timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
        return datetime >= checktime;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionMonth( const KFTimeData* timedata, KFDate& date )
    {
        auto datetime = ( date.GetMonth() * KFTimeEnum::OneMonthDay +  date.GetDay() ) * KFTimeEnum::OneDayMinute + date.GetHour() * KFTimeEnum::OneHourMinute + date.GetMinute();
        auto checktime = ( timedata->_month * KFTimeEnum::OneMonthDay + timedata->_day ) * KFTimeEnum::OneDayMinute + timedata->_hour * KFTimeEnum::OneHourMinute + timedata->_minute;
        return datetime >= checktime;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDate::CheckSectionYear( const KFTimeData* timedata, KFDate& date )
    {
        KFDate checkdate( timedata->_year, timedata->_month, timedata->_day, timedata->_hour, timedata->_month );
        return date.GetTime() >= checkdate.GetTime();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
