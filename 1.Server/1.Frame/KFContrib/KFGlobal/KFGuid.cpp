#include "KFrame.h"
#include "KFGuid.h"

namespace KFrame
{
#define __START_TIME__ 1542902400	// 项目开始时间 2018-11-23 0:0:0

    KFGuid::KFGuid( uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits )
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

    uint64 KFGuid::Make64Guid()
    {
        // time
        auto time = KFGlobal::Instance()->_real_time - __START_TIME__;
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
        KFAppID kfappid( KFGlobal::Instance()->_app_id );
        auto zoneid = kfappid._union._app_data._zone_id & _max_zone;
        auto workid = kfappid._union._app_data._worker_id & _max_seq;

        return ( time << _time_shift ) | ( zoneid << _zone_shift ) | ( workid << _worker_shift ) | _sequence;
    }

    void KFGuid::Print64Guid( uint64 guid )
    {
        auto sequence = ( guid & _max_seq );
        auto workerid = ( ( guid >> _worker_shift ) & _max_worker );
        auto zoneid = ( ( guid >> _zone_shift ) & _max_zone );
        auto time = ( ( guid >> _time_shift ) & _max_time ) + __START_TIME__;
        auto strtime = KFDate::GetTimeString( time );

        __LOG_INFO__( "guid[{}] time[{}] zoneid[{}] workerid[{}] sequence[{}]", guid, strtime, zoneid, workerid, sequence );
    }
}

