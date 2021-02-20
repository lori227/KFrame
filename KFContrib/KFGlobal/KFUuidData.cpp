#include "KFUuidData.hpp"

namespace KFrame
{
    void KFUuidData::InitData( uint64 project_time, uint32 time_bits, uint32 zone_bits, uint32 worker_bits, uint32 seq_bits )
    {
        static const int64 _base = -1;

        _start_time = project_time;

        _seq_bits = seq_bits;
        _max_seq = ~( _base << _seq_bits );

        _worker_bits = worker_bits;
        _max_worker = ~( _base << _worker_bits );
        _worker_shift = _seq_bits;

        _zone_bits = zone_bits;
        _max_zone = ~( _base << _zone_bits );
        _zone_shift = _worker_shift + _worker_bits;

        _time_bits = time_bits;
        _max_time = ~( _base << _time_bits );
        _time_shift = _zone_shift + _zone_bits;
    }
}

