#ifndef __KF_UUID_H__
#define __KF_UUID_H__

#include "KFInclude.h"

namespace KFrame
{
    /*
    +------+---------------+----------+----------------+----------+
    | sign | delta seconds | zone id  | worker node id | sequence |
    +------+---------------+----------+----------------+----------+
    | 1bit |     29bits    |  14bits  |     8bits      |  12bits  |
    +------+---------------+----------+----------------+----------+
    */

    // 默认
    // 1	符号位
    // 29	时间( 大概可以支持17年, 可以修改项目开始时间 )
    // 14	zoneid  可以支持16383个小区不会重复
    // 8	workerid 255个工作者进程不会重复
    // 12	序列号 同一进程1秒钟内超过4095个就会重复 -- 应该不可能, 除非你要疯
    // 可以保证同一模块生成的guid不相同, 不同模块可能会一样

    class KFUUID
    {
    public:
        KFUUID() = default;
        KFUUID( uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits );

        // 生产guid
        uint64 Make( uint32 zoneid, uint32 workerid, uint64 nowtime );

        // 打印guid
        void Print( uint64 guid );
    private:
        // 上一次时间
        uint64 _last_time{ 0 };

        // 序列号
        uint32 _sequence{ 0 };

        // 符号位
        uint32 _sign_bits{ 1 };

        // 时间位
        uint32 _time_bits{ 30 };
        uint64 _max_time{ 0x1FFFFFFF };
        uint32 _time_shift{ 33 };

        // 小区位
        uint32 _zone_bits{ 12 };
        uint64 _max_zone{ 0xFFF };
        uint32 _zone_shift{ 22 };

        // 进程位
        uint32 _worker_bits{ 8 };
        uint64 _max_worker{ 0xFF };
        uint32 _worker_shift{ 14 };

        // 序列位
        uint32 _seq_bits{ 13 };
        uint64 _max_seq{ 0x1FFF };
    };
}

#endif
