#include "KFClock.h"
#include "Poco/Clock.h"

namespace KFrame
{
    // 获得当前时间( 毫秒 ）
    uint64 KFClock::GetTime()
    {
        static Poco::Clock _clock;
        _clock.update();
        return _clock.microseconds() / 1000;
    }

    bool KFClock::CheckPassTime( uint64 starttime, uint64 keeptime )
    {
        return CheckPassTime( GetTime(), starttime, keeptime );
    }

    // 判断是否已经过了
    bool KFClock::CheckPassTime( uint64 now_time, uint64 starttime, uint64 keeptime )
    {
        uint64 endtime = static_cast<uint64>( starttime ) + static_cast<uint64>( keeptime );
        return _CheckPassTime( now_time, endtime );
    }

    // 获得消耗了的时间
    uint64 KFClock::GetPassTime( uint64 now_time, uint64 starttime )
    {
        return now_time - starttime;
    }

    // 获得剩余时间
    uint64 KFClock::GetLeftTime( uint64 now_time, uint64 starttime, uint64 keeptime )
    {
        uint64 passtime = GetPassTime( now_time, starttime );

        uint64 nLeftTime = 0;
        if ( passtime < keeptime )
        {
            nLeftTime = keeptime - passtime;
        }

        return nLeftTime;
    }

    bool KFClock::_CheckPassTime( uint64 now_time, uint64 endtime )
    {
        return now_time >= endtime;
    }
}