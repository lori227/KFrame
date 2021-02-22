#include "KFClock.h"

namespace KFrame
{
    // 获得当前时间( 毫秒 ）
    uint64 KFClock::GetTime()
    {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    }

    bool KFClock::CheckPassTime( uint64 start_time, uint64 keep_time )
    {
        return CheckPassTime( GetTime(), start_time, keep_time );
    }

    // 判断是否已经过了
    bool KFClock::CheckPassTime( uint64 now_time, uint64 start_time, uint64 keep_time )
    {
        uint64 end_time = static_cast<uint64>( start_time ) + static_cast<uint64>( keep_time );
        return _CheckPassTime( now_time, end_time );
    }

    // 获得消耗了的时间
    uint64 KFClock::GetPassTime( uint64 now_time, uint64 start_time )
    {
        return now_time - start_time;
    }

    // 获得剩余时间
    uint64 KFClock::GetLeftTime( uint64 now_time, uint64 start_time, uint64 keep_time )
    {
        uint64 pass_time = GetPassTime( now_time, start_time );

        uint64 nLeftTime = 0;
        if ( pass_time < keep_time )
        {
            nLeftTime = keep_time - pass_time;
        }

        return nLeftTime;
    }

    bool KFClock::_CheckPassTime( uint64 now_time, uint64 end_time )
    {
        return now_time >= end_time;
    }
}