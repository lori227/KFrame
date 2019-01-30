#include "KFrame.h"
#include "KFUUID.h"

namespace KFrame
{
#define __START_TIME__ 1542902400	// 项目开始时间 2018-11-23 0:0:0

    KFUUID::KFUUID( uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits )
    {
        _seq_bits = seqbits;
        _max_seq = ~( -1L << _seq_bits );

        _worker_bits = workerbits;
        _max_worker = ~( -1L << _worker_bits );
        _worker_shift = _seq_bits;

        _zone_bits = zonebits;
        _max_zone = ~( -1L << _zone_bits );
        _zone_shift = _worker_shift + _worker_bits;

        _time_bits = timebits;
        _max_time = ~( -1L << _time_bits );
        _time_shift = _zone_shift + _zone_bits;
    }

    uint64 KFUUID::Make( uint32 zoneid, uint32 workerid, uint64 nowtime )
    {
        // time
        auto time = nowtime - __START_TIME__;
        time &= _max_time;

        if ( time != _last_time )
        {
            // 确保同1秒内序列号是递增的
            _sequence = 0;
            _last_time = time;
        }

        // 序列号
        _sequence = ( _sequence + 1 ) & _max_seq;

        // appid
        zoneid &= _max_zone;
        workerid &= _max_seq;
        return ( time << _time_shift ) | ( zoneid << _zone_shift ) | ( workerid << _worker_shift ) | _sequence;
    }

    void KFUUID::Print( uint64 guid )
    {
        auto sequence = ( guid & _max_seq );
        auto workerid = ( ( guid >> _worker_shift ) & _max_worker );
        auto zoneid = ( ( guid >> _zone_shift ) & _max_zone );
        auto time = ( ( guid >> _time_shift ) & _max_time ) + __START_TIME__;
        auto strtime = KFDate::GetTimeString( time );

        __LOG_INFO__( "guid[{}] time[{}] zoneid[{}] workerid[{}] sequence[{}]", guid, strtime, zoneid, workerid, sequence );
    }
}

