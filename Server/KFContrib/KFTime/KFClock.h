#ifndef __GAME_TIME_H__
#define __GAME_TIME_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFClock
    {
    public:
        // 获得当前时间( 毫秒 ）
        static uint64 GetTime();

        // 判断时间是否超过
        static bool CheckPassTime( uint64 starttime, uint64 keeptime );

        // 判断是否已经过了
        static bool CheckPassTime( uint64 nowtime, uint64 starttime, uint64 keeptime );

        // 获得消耗了的时间
        static uint64 GetPassTime( uint64 nowtime, uint64 starttime );

        // 获得剩余时间
        static uint64 GetLeftTime( uint64 nowtime, uint64 starttime, uint64 keeptime );

    protected:
        //  时间判断
        static bool _CheckPassTime( uint64 nowtime, uint64 endtime );
    };
}

#endif