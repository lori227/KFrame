#ifndef __KF_UUID_SETTING_H__
#define __KF_UUID_SETTING_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFUuidData
    {
    public:
        // 设置数据排列格式
        void InitData( uint64 project_time, uint32 time_bits, uint32 zone_bits, uint32 worker_bits, uint32 seq_bits );

    public:
        // 时间戳缩减值
        uint64 _start_time = 1577808000;

        // 符号位
        uint32 _sign_bits = 1;

        // 时间位
        uint32 _time_bits = 30;
        uint64 _max_time = 0x1FFFFFFF;
        uint32 _time_shift = 33;

        // 小区位
        uint32 _zone_bits = 12;
        uint64 _max_zone = 0xFFF;
        uint32 _zone_shift = 22;

        // 进程位
        uint32 _worker_bits = 8;
        uint64 _max_worker = 0xFF;
        uint32 _worker_shift = 14;

        // 序列位
        uint32 _seq_bits = 13;
        uint64 _max_seq = 0x1FFF;
    };
}

#endif
