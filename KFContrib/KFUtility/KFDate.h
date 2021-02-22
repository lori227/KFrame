#ifndef __KF_DATE_H__
#define __KF_DATE_H__
/************************************************************************

					现实时间  单位 : 秒


************************************************************************/
#include "KFDefine.h"
typedef struct tm _tm;

namespace KFrame
{
    namespace KFTimeEnum
    {
        enum EConstDefine
        {
            SinceYear = 1900,

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
            None = 0,
            Minute = 1,	// 分钟
            Hour = 2,	// 小时
            Day = 3,	// 每天
            Week = 4,	// 每周
            Month = 5,	// 每月
            Year = 6,	// 每年
            Ever = 7,	// 永久
            Max,
            ////////////////////////////////////////////////////////////////////////
            Loop = 1,		// 时间循环
            Section = 2,	// 时间区间
            ////////////////////////////////////////////////////////////////////////
        };
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFTimeData
    {
    public:
        // 年
        uint32 _year = 0u;

        // 月
        uint32 _month = 0u;

        // 天
        uint32 _day = 0u;

        // 星期
        uint32 _day_of_week = 0u;

        // 小时
        uint32 _hour = 0u;

        // 分钟
        uint32 _minute = 0u;

        // 时间类型( 逻辑使用 )
        uint32 _flag = 0u;
    };

    // 时间区间
    class KFTimeSection
    {
    public:
        // 开始时间
        KFTimeData _start_time_data;

        // 结束时间
        KFTimeData _finish_time_data;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 系统时间
    class KFDate
    {
    public:
        KFDate();
        KFDate( uint64 time );
        KFDate( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 min, uint32 second = 0 );

        // 获得时间
        uint64 GetTime() const;
        void SetTime( uint64 time );
        void AddTime( uint64 time );

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

        // 转换时间戳
        static uint64 FromString( const std::string& ymd );

        // 判断同一天
        static bool CheckSameDay( uint64 last_time, uint64 now_time );
        static bool CheckSameDay( KFDate& last_date, KFDate& now_date );

        // 判断在时间内
        static bool CheckInTime( uint64 start_time, uint64 end_time, uint64 now_time );

        // 判断是否过期了
        static bool CheckTimeout( uint64 start_time, uint64 keep_time );
        static bool CheckTimeout( uint64 now_time, uint64 start_time, uint64 keep_time );

        // 计算剩余时间
        static uint64 CalcLeftTime( uint64 now_time, uint64 start_time, uint32 keep_time );

        // 判断是否过了时间点
        static bool CheckPassTime( uint64 now_time, uint64 next_time );
        static bool CheckPassTime( uint32 year, uint32 month, uint32 day, uint32 hour, uint32 minute );

        // 获得0点0分的时间
        static uint64 CalcZeroTime( uint64 time, int32 day_count = 0 );

        // 获得上次重置时间
        static uint64 CalcTimeData( const KFTimeData* time_data, uint64 time, int32 count = 0 );

        // 判断时间( 循环时间, 需要上一次时间参与判断 )
        static bool CheckLoopTimeData( const KFTimeData* time_data, uint64 last_time, uint64 now_time );
        static bool CheckLoopTimeData( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date );

        // 判断时间( 区间时间, 是否过了某一个时间点 )
        static bool CheckInTimeSection( const KFTimeSection* time_section, uint64 time );
        static bool CheckInTimeSection( const KFTimeSection* time_section, KFDate& date );
        static bool CheckSectionTimeData( const KFTimeData* time_data, KFDate& date );
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        // 转换时间
        void ConvertTimeDate();

        // 判断是否过了分钟
        static bool CheckLoopMinute( const KFTimeData* time_data, uint64 last_time, uint64 now_time );
        static bool CheckLoopMinute( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date );

        // 判断是否过了小时
        static bool CheckLoopHour( const KFTimeData* time_data, uint64 last_time, uint64 now_time );
        static bool CheckLoopHour( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date );

        // 判断是否过了天
        static bool CheckLoopDay( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date );

        // 判断是否过了周
        static bool CheckLoopWeek( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date );

        // 判断是否过了月
        static bool CheckLoopMonth( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date );

        // 判断是否过了一年
        static bool CheckLoopYear( const KFTimeData* time_data, KFDate& last_date, KFDate& now_date );

        // 判断是否过了永久
        static bool CheckLoopEver( uint64 last_time );
        static bool CheckLoopEver( KFDate& last_date );

        // 判断是否过了某分钟
        static bool CheckSectionMinute( const KFTimeData* time_data, KFDate& date );

        // 判断是否过了某小时某分钟
        static bool CheckSectionHour( const KFTimeData* time_data, KFDate& date );

        // 判断是否过了某天某小时某分钟
        static bool CheckSectionDay( const KFTimeData* time_data, KFDate& date );

        // 判断是否过了星期几的某小时某分钟
        static bool CheckSectionWeek( const KFTimeData* time_data, KFDate& date );

        // 判断是否过了某月某号的某小时某分钟
        static bool CheckSectionMonth( const KFTimeData* time_data, KFDate& date );

        // 判断是否过了某年某月某号的某小时某分钟
        static bool CheckSectionYear( const KFTimeData* time_data, KFDate& date );

    private:
        time_t _time;
        _tm _tm_date;
    };


}




#endif