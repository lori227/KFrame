#include "KFRand.hpp"

namespace KFrame
{
    KFRand::KFRand()
    {
        _result_count = 0;
        Init( static_cast< uint32 >( time( nullptr ) ) );
    }

    KFRand::~KFRand()
    {

    }

    uint32 KFRand::Rand32()
    {
        if ( _result_count == 0 )
        {
            SetRand();
            _result_count = 256;
        }

        return _result[ --_result_count ];
    }

    double KFRand::RandFloat()
    {
        return ( ( double )Rand32() / 4294967296.0 );
    }

    void KFRand::Init( uint32 seed )
    {
        seed |= 1;

        for ( int32 i = 0; i < 256; i++ )  // initialize memory
        {
            seed = 16807 * ( seed % 127773 ) - 2836 * ( seed / 127773 );
            _seed[ i ] = seed;
        }

        SetRand();      // get first set of results
        _result_count = 256;
    }

    void KFRand::SetRand()
    {
        register uint32 a = _aa, b = _bb, x, y, i;

        for ( i = 0; i < 256; ++i )
        {
            x = _seed[ i ];
            a = ( ( a << 19 ) ^ ( a >> 13 ) ) + _seed[ ( i + 128 ) & 255 ];
            _seed[ i ] = y = _seed[ x & 255 ] + a + b;
            _result[ i ] = b = _seed[ ( y >> 8 ) & 255 ] + x;
        }
        _bb = b;
        _aa = a;
    }
}