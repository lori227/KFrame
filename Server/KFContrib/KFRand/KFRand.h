#ifndef __KF_RAND_H__
#define __KF_RAND_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFRand
    {
    public:
        // 随机整数32
        static uint32 STRand32();

        // 随机浮点数
        static double STRandFloat();

        // 随机
        static uint32 STRandDistrict( uint32 minnumber, uint32 maxnumber, uint32 district );

        // 随机
        static int32 STRandRange( uint32 number );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机整数32
        static uint32 MTRand32();

        // 随机浮点数
        static double MTRandFloat();

        // 随机
        static uint32 MTRandDistrict( uint32 minnumber, uint32 maxnumber, uint32 district );

        // 随机
        static int32 MTRandRange( uint32 number );
    };
}



#endif