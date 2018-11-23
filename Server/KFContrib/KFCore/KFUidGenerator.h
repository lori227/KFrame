#ifndef __KF_UID_GENERATOR_H__
#define __KF_UID_GENERATOR_H__

#include "KFrame.h"

namespace KFrame
{

    class KFBitsAllocator
    {
    public:
        static const int TOTAL_BITS = 1 << 6;

        KFBitsAllocator( int ts_bits, int worker_bits, int seq_bits ) :
            _sign_bits( 1 ),
            _timestamp_bits( ts_bits ),
            _worker_id_bits( worker_bits ),
            _sequence_bits{ seq_bits }
        {
            int alloc_total_bit = _sign_bits + ts_bits + worker_bits + seq_bits;
            assert( alloc_total_bit == TOTAL_BITS ); //allocate not enough 64 bits

            _max_delta_seconds = ~( -1L << _timestamp_bits );
            _max_worker_id = ~( -1L << _worker_id_bits );
            _max_sequence = ~( -1L << _sequence_bits );

            _timestamp_shift = _worker_id_bits + _sequence_bits;
            _worker_id_shift = _sequence_bits;
        }

        int64 Alloc( int64 delta_seconds, int64 worker_id, int64 seq )
        {
            return ( delta_seconds << _timestamp_shift ) | ( worker_id << _worker_id_shift ) | seq;
        }

        int GetTotalBits()
        {
            return TOTAL_BITS;
        }

        int GetSignBits()
        {
            return _sign_bits;
        }

        int GetTimestampBits()
        {
            return _timestamp_bits;
        }

        int GetWorkerIdBits()
        {
            return _worker_id_bits;
        }

        int GetSequenceBits()
        {
            return _sequence_bits;
        }

        int64 GetMaxSequence()
        {
            return _max_sequence;
        }

        int64 GetMaxDeltaSeconds()
        {
            return _max_delta_seconds;
        }

    private:
        int _sign_bits{ 1 };
        int _timestamp_bits{ 0 };
        int _worker_id_bits{ 0 };
        int _sequence_bits{ 0 };

        int64 _max_delta_seconds{ 0 };
        int64 _max_worker_id{ 0 };
        int64 _max_sequence{ 0 };

        int _timestamp_shift{ 0 };
        int _worker_id_shift{ 0 };
    };

    /*
    +------+---------------+----------------+----------+
    | sign | delta seconds | worker node id | sequence |
    +------+---------------+----------------+----------+
    | 1bit |     29bits    |     24bits     |  10bits  |
    */

    class KFUidGenerator
    {
    public:
        static const int time_bits = 29;
        static const int worker_bit = 24;
        static const int seq_bits = 10; //generate pow(2, 10) per second
        static const int64 uid_epoch = 1542643200L; //2018/11/20 00:00:00

        KFUidGenerator()
        {
            _bits_alloc = __KF_NEW__( KFBitsAllocator, time_bits, worker_bit, seq_bits );
        }

        ~KFUidGenerator()
        {
            __KF_DELETE__( _bits_alloc );
        }

        int64 GetUID( const int64 worker_id )
        {
            return NextID( worker_id );
        }

        std::string ParseUID( const int64 uid )
        {
            int64 total_bits = _bits_alloc->GetTotalBits();
            int64 sign_bits = _bits_alloc->GetSignBits();
            int64 timestamp_bits = _bits_alloc->GetTimestampBits();
            int64 worker_id_bits = _bits_alloc->GetWorkerIdBits();
            int64 sequence_bits = _bits_alloc->GetSequenceBits();

            int64 seq = ( uint64_t( uid << ( total_bits - sequence_bits ) ) ) >> ( total_bits - sequence_bits );
            int64 worker_id = ( uint64_t( uid << ( sign_bits + timestamp_bits ) ) ) >> ( total_bits - worker_id_bits );
            int64 delta_seconds = ( uint64_t( uid ) ) >> ( worker_id_bits + sequence_bits );

            return __FORMAT__( "UID={} timestamp={} workerid={} sequence={}", uid, KFDate::GetTimeString( delta_seconds + uid_epoch ), worker_id, seq );
        }

    protected:
        int64 NextID( const int64 worker_id )
        {
            int64 cur_second = GetCurrentSecond();
            assert( cur_second >= _last_second );

            if ( cur_second == _last_second )
            {
                _sequence = ( _sequence + 1 ) & _bits_alloc->GetMaxSequence();
                if ( _sequence == 0 )
                {
                    cur_second = GetNextSecond( _last_second );
                }
            }
            else
            {
                _sequence = 0;
            }

            _last_second = cur_second;
            return _bits_alloc->Alloc( cur_second - uid_epoch, worker_id, _sequence );
        }


        int64 GetCurrentSecond()
        {
            int64 cur_second = KFDate::GetTimeEx();
            assert( ( cur_second - uid_epoch ) <= _bits_alloc->GetMaxDeltaSeconds() );

            return cur_second;
        }

        int64 GetNextSecond( int64 last_ts )
        {
            int64 ts = GetCurrentSecond();
            while ( ts <= last_ts )
            {
                ts = GetCurrentSecond();
            }

            return ts;
        }

    private:
        int64 _sequence{ 0L };
        int64 _last_second{ -1L };

        KFBitsAllocator* _bits_alloc;
    };

    class KFUidGeneratorThreadSafe : public KFUidGenerator
    {
    public:
        int64 GetUID( const int64 worker_id )
        {
            std::lock_guard<std::mutex> lock( mutex_ );
            return NextID( worker_id );
        }

    private:
        mutable std::mutex mutex_;
    };

}

#endif //__KF_UID_GENERATOR_H__