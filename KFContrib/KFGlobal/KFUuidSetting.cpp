#include "KFUuidSetting.hpp"

namespace KFrame
{
    void KFUuidSetting::InitSetting( uint64 projecttime, uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits )
    {
        static const int64 _base = -1;

        _start_time = projecttime;

        _seq_bits = seqbits;
        _max_seq = ~( _base << _seq_bits );

        _worker_bits = workerbits;
        _max_worker = ~( _base << _worker_bits );
        _worker_shift = _seq_bits;

        _zone_bits = zonebits;
        _max_zone = ~( _base << _zone_bits );
        _zone_shift = _worker_shift + _worker_bits;

        _time_bits = timebits;
        _max_time = ~( _base << _time_bits );
        _time_shift = _zone_shift + _zone_bits;
    }
}

