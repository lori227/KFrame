#ifndef __KF_DATE_H__
#define __KF_DATE_H__
/************************************************************************

					现实时间  单位 : 秒


************************************************************************/
#include "KFInclude.h"
typedef struct tm _tm;

namespace KFrame
{
    namespace TimeEnum
    {
        enum EConstDefine
        {
            SinceYear = 1970,

            OneSecondMicSecond = 1000,
            OneMinuteSecond = 60,
            OneMinuteMicSecond = 60 * OneSecondMicSecond,
            OneHourMinute = 60,
            OneHourSecond = OneHourMinute * OneMinuteSecond,
            OneHourMicSecond = OneHourSecond * OneSecondMicSecond,
            OneDayHour = 24,
            OneDayMinute = OneDayHour * OneHourMinute,
            OneDaySecond = OneDayMinute * OneMinuteSecond,
            OneDayMicSecond = OneDaySecond * OneSecondMicSecond,
            OneWeekDay = 7,
            OneWeekHour = OneWeekDay * OneDayHour,
            OneWeekMinute = OneWeekHour * OneHourMinute,
            OneWeekSecond = OneWeekMinute * OneMinuteSecond,

            OneMonthDay = 30,
            ////////////////////////////////////////////////////////////////////////
            Day = 1,
            Week = 2,
            Month = 3,
            Year = 4,
            Max,
        };
    }

    // 时间间隔
    class TimeSpan
    {
    public:
        TimeSpan() : _time( 0 ) {}
        TimeSpan( uint64 time ) : _time( time ) {}

        uint64 GetTime() const {
            return _time;
        }

        // 天数
        uint32 GetDays() const {
            return static_cast< uint32 >( _time / TimeEnum::OneDaySecond );
        }

        // 小时数
        uint32 GetHours() const {
            return GetTotalHours() - GetDays() * TimeEnum::OneDayHour;
        }
        uint32 GetTotalHours() const {
            return static_cast< uint32 >( _time / TimeEnum::OneHourSecond );
        }

        // 分钟数
        uint32 GetMinutes() const {
            return GetTotalMinutes() - GetTotalHours() * TimeEnum::OneHourMinute;
        }
        uint32 GetTotalMinutes() const {
            return static_cast< uint32 >( _time / TimeEnum::OneMinuteSecond );
        }

        // 秒数
        uint32 GetSeconds() const {
            return _time % TimeEnum::OneMinuteSecond;
        }
        uint64 GetTotalSeconds() const {
            return _time;
        }

    private:
        // 时间
        uint64 _time;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 系统时间
    class KFDate
    {
    public:
        KFDate();
        KFDate( uint64 time );
        KFDate( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 min, uint32 second = 0 );

        KFDate& operator += ( TimeSpan time );
        KFDate& operator -= ( TimeSpan time );
        KFDate operator + ( TimeSpan time ) const;
        KFDate operator - ( TimeSpan time ) const;

        KFDate& operator += ( uint64 time );
        KFDate& operator -= ( uint64 time );
        KFDate operator + ( uint64 time ) const;
        KFDate operator - ( uint64 time ) const;

        bool operator == ( KFDate time ) const;
        bool operator != ( KFDate time ) const;
        bool operator <= ( KFDate time ) const;
        bool operator >= ( KFDate time ) const;
        bool operator < ( KFDate time ) const;
        bool operator > ( KFDate time ) const;

        TimeSpan operator - ( KFDate time ) const;

        // 获得时间
        uint64 GetTime() const {
            return _time;
        }
        void SetTime( uint64 time ) {
            _time = time;
        }

        // 获得具体年月日时间
        uint32 GetYear() const;
        uint32 GetMonth() const;
        uint32 GetDay() const;
        uint32 GetHour() const;
        uint32 GetMinute() const;
        uint32 GetSecond() const;
        uint32 GetDayOfWeek() const;
        uint32 GetDayOfYear() const;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::string GetTimeString();
        static std::string GetTimeString( uint64 time );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 获得当前时间
        static KFDate GetDate();
        static uint64 GetTimeEx();

        // 判断在时间内
        static bool CheckInTime( uint64 start, uint64 end, uint64 now );

        // 判断时间是否已经过了
        static bool CheckPassTime( uint64 starttime, uint64 keeptime );
        static bool CheckPassTime( uint64 nowtime, uint64 starttime, uint64 keeptime );

        // 获得剩余时间
        static uint64 GetLeftTime( uint64 nowtime, uint64 starttime, uint32 keeptime );

        // 判断是否过了一年
        static bool CheckPassYear( uint64 lasttime, uint64 nowtime );

        // 判断是否过了一个月
        static bool CheckPassMonth( uint64 lasttime, uint64 nowtime );

        // 判断是否过了某一天
        static bool CheckPassMonth( uint64 lasttime, uint64 nowtime, uint32 day );

        // 判断是否过了一天
        static bool CheckPassDay( uint64 lasttime, uint64 nowtime );
        static bool CheckSameDay( uint64 lasttime, uint64 nowtime );

        // 判断是否过了某一天
        static bool CheckPassDay( uint64 lasttime, uint64 nowtime, uint32 hour );

        // 判断是否过了一周
        static bool CheckPassWeek( uint64 lasttime, uint64 nowtime );

        // 判断是否过了周急
        static bool CheckPassWeek( uint64 lasttime, uint64 nowtime, uint32 dayofweek );

        // 判断是否过了一小时
        static bool CheckPassHour( uint64 lasttime, uint64 nowtime );

        // 判断时间
        static bool CheckTime( uint32 type, uint32 time, uint64 lasttime, uint64 nowtime );

        // 判断是否过了时间
        static bool CheckPassTime( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 minute );

        // 转换时间戳
        static uint64 TimeFormate( const std::string& ymd, const std::string& split = "-" );

    protected:
        _tm* LocalTime() const;

    private:
        uint64 _time;
    };
}




#endif