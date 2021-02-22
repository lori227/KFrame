#ifndef __GAME_TIME_H__
#define __GAME_TIME_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFClock
    {
    public:
        // 获得当前时间( 毫秒 ）
        static uint64 GetTime();

        // 判断时间是否超过
        static bool CheckPassTime( uint64 start_time, uint64 keep_time );

        // 判断是否已经过了
        static bool CheckPassTime( uint64 now_time, uint64 start_time, uint64 keep_time );

        // 获得消耗了的时间
        static uint64 GetPassTime( uint64 now_time, uint64 start_time );

        // 获得剩余时间
        static uint64 GetLeftTime( uint64 now_time, uint64 start_time, uint64 keep_time );

    protected:
        //  时间判断
        static bool _CheckPassTime( uint64 now_time, uint64 end_time );
    };
}

#endif