#ifndef __KF_RAND_H__
#define __KF_RAND_H__

#include "KFInclude.h"

namespace KFrame
{
    // 单线程
    class KFRand
    {
    public:
        KFRand();
        ~KFRand();

        // 随机整数32
        uint32 Rand32();

        // 随机浮点数
        double RandFloat();

    protected:

        // 初始化
        void Init( uint32 seed );

        // 设置随机数
        void SetRand();

    private:
        // 结果数量
        uint32 _result_count;

        uint32 _seed[ 256 ], _result[ 256 ], _aa, _bb;
    };
}



#endif