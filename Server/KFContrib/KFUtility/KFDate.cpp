#include "KFDate.h"
#include <stdio.h>
#include <time.h>

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
        _tm_date.tm_year = static_cast< int32 >( year - KFTimeEnum::SinceYear );     // tm_year is 1900 based
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

        char buff[ 128 ] = "";
        sprintf( buff, "%04d-%02d-%02d %02d:%02d:%02d",
                 year, month, day, hour, minute, second );
        return buff;
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

    bool KFDate::CheckPassHour( uint64 lasttime, uint64 nowtime )
    {
        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );

        if ( nowdate.GetMonth() != lastdate.GetMonth() )
        {
            return true;
        }

        if ( nowdate.GetDay() != lastdate.GetDay() )
        {
            return true;
        }

        return nowdate.GetHour() != lastdate.GetHour();
    }

    bool KFDate::CheckSameDay( uint64 lasttime, uint64 nowtime )
    {
        return !CheckPassDay( lasttime, nowtime, 0 );
    }

    bool KFDate::CheckPassDay( uint64 lasttime, uint64 nowtime, uint32 hour )
    {
        // 大于一天
        auto timedistance = nowtime - lasttime;
        if ( timedistance >= KFTimeEnum::OneDaySecond )
        {
            return true;
        }

        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );

        auto nowhour = nowdate.GetHour();
        auto nowday = nowdate.GetDay();

        auto lasthour = lastdate.GetHour();
        auto lastday = lastdate.GetDay();

        // 不同天了
        if ( nowday != lastday )
        {
            return lasthour <= hour || nowhour >= hour;
        }

        // 同一天, 小于1小时
        if ( timedistance < KFTimeEnum::OneHourSecond )
        {
            return false;
        }

        return ( lasthour < hour && nowhour >= hour );
    }

    bool KFDate::CheckPassWeek( uint64 lasttime, uint64 nowtime, uint32 dayofweek, uint32 hour )
    {
        // 跨度超过1周
        auto timedistance = nowtime - lasttime ;
        if ( timedistance >= KFTimeEnum::OneWeekSecond )
        {
            return true;
        }

        KFDate nowdate( nowtime );
        KFDate lastdate( lasttime );

        auto nowdayofweek = nowdate.GetDayOfWeek();
        auto lastdayofweek = lastdate.GetDayOfWeek();

        auto nowhour = nowdate.GetHour();
        auto lasthour = lastdate.GetHour();

        auto nowweek = nowtime / KFTimeEnum::OneWeekSecond;
        auto lastweek = lasttime / KFTimeEnum::OneWeekSecond;

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

        return nowdate.GetYear() != lastdate.GetYear();
    }

    bool KFDate::CheckTime( uint32 type, uint32 value, uint32 hour, uint64 lasttime, uint64 nowtime )
    {
        switch ( type )
        {
        case KFTimeEnum::Hour:
            return KFDate::CheckPassHour( lasttime, nowtime );
            break;
        case KFTimeEnum::Day:		// 判断时间
            return KFDate::CheckPassDay( lasttime, nowtime, hour );
            break;
        case KFTimeEnum::Week:	// 判断时间
            return KFDate::CheckPassWeek( lasttime, nowtime, value, hour );
            break;
        case KFTimeEnum::Month:	// 月份
            return KFDate::CheckPassMonth( lasttime, nowtime, value, hour );
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

    uint64 KFDate::TimeFormate( const std::string& ymd, const std::string& split /* = "-" */ )
    {
        if ( ymd.empty() )
        {
            return 0;
        }

        // 必须按照格式来填写时间
        int32 year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
        sscanf( ymd.c_str(), "%d-%d-%d-%d:%d:%d", &year, &month, &day, &hour, &minute, &second );

        KFDate kfdate( year, month, day, hour, minute, second );
        return kfdate.GetTime();
    }
}
