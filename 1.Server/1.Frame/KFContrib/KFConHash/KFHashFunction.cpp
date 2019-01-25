#include "KFHashFunction.hpp"
#include "KFCrypto/KFCrypto.h"

namespace KFrame
{
    uint32 MD5HashFunction::GetHashValue( const std::string& data )
    {
        auto md5value = KFCrypto::Md5Encode( data );
        auto digest = md5value.c_str();

        uint32 hash = 0;
        for ( auto i = 0; i < 4; i++ )
        {
            hash += ( ( uint32 )( digest[ i * 4 + 3 ] & 0xFF ) << 24 )
                    | ( ( uint32 )( digest[ i * 4 + 2 ] & 0xFF ) << 16 )
                    | ( ( uint32 )( digest[ i * 4 + 1 ] & 0xFF ) << 8 )
                    | ( ( uint32 )( digest[ i * 4 + 0 ] & 0xFF ) );
        }

        return hash;
    }
}

