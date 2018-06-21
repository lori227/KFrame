#include "KFDate.h"
#include <stdio.h>
#include <time.h>

namespace KFrame
{
	KFDate::KFDate() : _time( 0 )
	{

	}

	KFDate::KFDate( uint64 time ) : _time( time )
	{

	}

	KFDate::KFDate( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 min, uint32 second /* = 0 */ )
	{
		struct tm atm;

		atm.tm_year = static_cast< int32 >( year - TimeEnum::SinceYear );     // tm_year is 1900 based
		atm.tm_mon = static_cast< int32 >( month - 1 );        // tm_mon is 0 based
		atm.tm_mday = static_cast< int32 >( day );
		atm.tm_hour = static_cast< int32 >( hour );
		atm.tm_min = static_cast< int32 >( min );
		atm.tm_sec = static_cast< int32 >( second );
		atm.tm_isdst = -1;

		_time = static_cast< uint64 >( mktime( &atm ) );
	}

	KFDate& KFDate::operator += ( TimeSpan time )
	{
		_time += time.GetTotalSeconds();
		return *this;
	}

	KFDate& KFDate::operator -= ( TimeSpan time )
	{
		_time -= time.GetTotalSeconds();
		return *this;
	}

	KFDate KFDate::operator + ( TimeSpan time ) const
	{
		return KFDate( _time + time.GetTotalSeconds() );
	}

	KFDate KFDate::operator - ( TimeSpan time ) const
	{
		return KFDate( _time - time.GetTotalSeconds() );
	}

	KFDate& KFDate::operator += ( uint64 time )
	{
		_time += time;
		return *this;
	}

	KFDate& KFDate::operator -= ( uint64 time )
	{
		_time -= time;
		return *this;
	}

	KFDate KFDate::operator + ( uint64 time ) const
	{
		return KFDate( _time + time );
	}

	KFDate KFDate::operator - ( uint64 time ) const
	{
		return KFDate( _time - time );
	}

	TimeSpan KFDate::operator - ( KFDate time ) const
	{
		return TimeSpan( _time - time.GetTime() );
	}

	bool KFDate::operator == ( KFDate time ) const
	{
		return _time == time.GetTime();
	}

	bool KFDate::operator != ( KFDate time ) const
	{
		return _time != time.GetTime();
	}

	bool KFDate::operator <= ( KFDate time ) const
	{
		return _time <= time.GetTime();
	}

	bool KFDate::operator >= ( KFDate time ) const
	{
		return _time >= time.GetTime();
	}

	bool KFDate::operator < ( KFDate time ) const
	{
		return _time < time.GetTime();
	}

	bool KFDate::operator > ( KFDate time ) const
	{
		return _time > time.GetTime();
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

	uint64 KFDate::GetTimeEx()
	{
		KFDate nowtime = KFDate::GetDate();
		return nowtime.GetTime();
	}

	_tm* KFDate::LocalTime() const
	{
		time_t time = _time;
		return localtime( &time );
	}

	uint32 KFDate::GetYear() const
	{
		auto ptm = LocalTime();
		return ptm->tm_year + TimeEnum::SinceYear;
	}

	uint32 KFDate::GetMonth() const
	{
		auto ptm = LocalTime();
		return ptm->tm_mon + 1;
	}

	uint32 KFDate::GetDay() const
	{
		auto ptm = LocalTime();
		return ptm->tm_mday;
	}

	uint32 KFDate::GetHour() const
	{
		auto ptm = LocalTime();
		return ptm->tm_hour;
	}

	uint32 KFDate::GetMinute() const
	{
		auto ptm = LocalTime();
		return ptm->tm_min;
	}

	uint32 KFDate::GetSecond() const
	{
		auto ptm = LocalTime();
		return ptm->tm_sec;
	}

	uint32 KFDate::GetDayOfWeek() const
	{
		auto ptm = LocalTime();
		return ptm->tm_wday == 0 ? 7 : ptm->tm_wday;
	}

	uint32 KFDate::GetDayOfYear() const
	{
		auto ptm = LocalTime();
		return ptm->tm_yday;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string KFDate::GetTimeString( uint64 time )
	{
		auto ptm = localtime( reinterpret_cast< time_t* >( &time ) );

		uint64 year = ptm->tm_year + TimeEnum::SinceYear;
		uint64 month = ptm->tm_mon + 1;
		uint64 day = ptm->tm_mday;
		uint64 hour = ptm->tm_hour;
		uint64 minute = ptm->tm_min;
		uint64 second = ptm->tm_sec;

		std::stringstream ss( std::stringstream::in | std::stringstream::out );
		ss << std::setfill( '0' )
			<< std::setw( 4 ) << year << "-"
			<< std::setw( 2 ) << month << "-"
			<< std::setw( 2 ) << day << " "
			<< std::setw( 2 ) << hour << ":"
			<< std::setw( 2 ) << minute << ":"
			<< std::setw( 2 ) << second;

		return ss.str();
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

	bool KFDate::CheckPassYear( uint64 lasttime, uint64 nowtime )
	{
		KFDate nowdate( nowtime );
		KFDate lastdate( lasttime );

		return nowdate.GetYear() != lastdate.GetYear();
	}

	bool KFDate::CheckPassMonth( uint64 lasttime, uint64 nowtime )
	{
		KFDate nowdate( nowtime );
		KFDate lastdate( lasttime );

		if ( nowdate.GetYear() != lastdate.GetYear() )
		{
			return true;
		}

		return nowdate.GetMonth() != lastdate.GetMonth();
	}

	bool KFDate::CheckPassMonth( uint64 lasttime, uint64 nowtime, uint32 day )
	{
		KFDate nowdate( nowtime );
		KFDate lastdate( lasttime );

		// 是同一天
		if ( nowdate.GetDay() == day )
		{
			return true;
		}

		auto monthdistance = ( nowdate.GetYear() - lastdate.GetYear() ) * 12 + nowdate.GetMonth() - lastdate.GetMonth();
		if ( monthdistance >= 2 )
		{
			return true;
		}

		auto nowday = nowdate.GetDay();
		auto lastday = lastdate.GetDay();

		// 同一个月
		if ( monthdistance == 0 )
		{
			return ( lastday < day && nowday > day );
		}

		// 不同月
		return ( lastday < day || nowday > day );
	}

	bool KFDate::CheckPassDay( uint64 lasttime, uint64 nowtime )
	{
		KFDate nowdate( nowtime );
		KFDate lastdate( lasttime );

		if ( nowdate.GetYear() != lastdate.GetYear() )
		{
			return true;
		}

		if ( nowdate.GetMonth() != lastdate.GetMonth() )
		{
			return true;
		}

		return nowdate.GetDay() != lastdate.GetDay();
	}

	bool KFDate::CheckSameDay( uint64 lasttime, uint64 nowtime )
	{
		return !CheckPassDay( lasttime, nowtime );
	}

	bool KFDate::CheckPassDay( uint64 lasttime, uint64 nowtime, uint32 hour )
	{
		KFDate nowdate( nowtime );
		KFDate lastdate( lasttime );

		TimeSpan timedistance = nowdate - lastdate;

		// 大于一天
		if ( timedistance.GetTotalSeconds() >= TimeEnum::OneDaySecond )
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

		// 同一天, 小于1小时
		if ( timedistance.GetTotalSeconds() < TimeEnum::OneHourSecond )
		{
			return false;
		}

		return ( lasthour < hour && nowhour >= hour );
	}

	bool KFDate::CheckPassWeek( uint64 lasttime, uint64 nowtime )
	{
		return CheckPassWeek( lasttime, nowtime, 1 );
	}

	bool KFDate::CheckPassWeek( uint64 lasttime, uint64 nowtime, uint32 dayofweek )
	{
		KFDate nowdate( nowtime );
		KFDate lastdate( lasttime );

		// 跨度超过1周
		TimeSpan timedistance = nowdate - lastdate;
		if ( timedistance.GetTotalSeconds() >= TimeEnum::OneWeekSecond )
		{
			return true;
		}

		if ( timedistance.GetTotalSeconds() < TimeEnum::OneDaySecond )
		{
			return false;
		}

		auto nowdayofweek = nowdate.GetDayOfWeek();
		auto lastdayofweek = lastdate.GetDayOfWeek();
		if ( nowdayofweek == dayofweek )
		{
			return true;
		}

		// 不同周
		if ( nowdayofweek < lastdayofweek )
		{
			return lastdayofweek < dayofweek || nowdayofweek >= dayofweek;
		}

		// 在同1周
		return ( lastdayofweek < dayofweek && nowdayofweek >= dayofweek );
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

	bool KFDate::CheckTime( uint32 type, uint32 time, uint64 lasttime, uint64 nowtime )
	{
		switch ( type )
		{
		case TimeEnum::Day:		// 判断时间
			return KFDate::CheckPassDay( lasttime, nowtime, time );
			break;
		case TimeEnum::Week:	// 判断时间
			return KFDate::CheckPassWeek( lasttime, nowtime, time );
			break;
		case TimeEnum::Month:	// 月份
			return KFDate::CheckPassMonth( lasttime, nowtime, time );
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
		uint32 nowyear = ptm->tm_year + TimeEnum::SinceYear;
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
