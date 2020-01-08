#ifndef __KF_UUID_H__
#define __KF_UUID_H__

#include "KFInclude.h"

namespace KFrame
{
    /*
    +------+---------------+----------+----------------+----------+
    | sign | delta seconds | zone id  | worker node id | sequence |
    +------+---------------+----------+----------------+----------+
    | 1bit |     29bits    |  10bits  |     10bits      |  14bits  |
    +------+---------------+----------+----------------+----------+
    */

    // 默认
    // 1	符号位
    // 29	时间( 大概可以支持17年, 可以修改项目开始时间 )
    // 10	zoneid  可以支持1023个小区不会重复
    // 10	workerid 1023个工作者进程不会重复
    // 14	序列号 同一进程1秒钟内超过‭16383‬个就会重复 -- 应该不可能, 除非你要疯
    // 可以保证同一模块生成的guid不相同, 不同模块可能会一样
    class KFUUIDSetting
    {
    public:
        // 初始化时间
        void InitStartTime( uint64 starttime );

        // 设置数据排列格式
        void InitSetting( uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits );
    public:
        // 时间戳缩减值
        uint64 _start_time{ 1577808000 };

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

    class KFUUID
    {
    public:
        KFUUID() = default;
        ~KFUUID() = default;

        // 初始化时间
        void InitStartTime( uint64 starttime );

        // 设置数据排列格式
        void InitSetting( uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////
        // 生产guid
        uint64 Make( uint32 zoneid, uint32 workerid, uint64 nowtime );

        // 获得zoneid
        uint32 ZoneId( uint64 uuid );

        // 打印guid
        void Print( uint64 uuid );

    private:
        // uuid 配置
        KFUUIDSetting _setting;

        // 上一次时间
        uint64 _last_time{ 0 };

        // 序列号
        uint32 _sequence{ 0 };
    };
}

#endif
