#include "KFDecode.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    static uint8 _decode_bit_mask[ 5 ] = { 0xfc, 0xf8, 0xf0, 0xe0, 0xc0 };

    uint32 KFDecode::UByteToString( const uint8* source, uint32 sourcelength, int8* target, uint32 targetlength )
    {
        uint8 made = 0;
        uint8 rest = 0;
        uint32 targetposition = 0;
        uint32 restcount = 0;

        for ( uint32 i = 0; i < sourcelength; ++i )
        {
            if ( targetposition >= targetlength )
            {
                break;
            }

            made = ( ( rest | ( source[ i ] >> ( 2 + restcount ) ) ) & 0x3f );
            rest = ( ( ( source[ i ] << ( 8 - ( 2 + restcount ) ) ) >> 2 ) & 0x3f );

            restcount += 2;

            if ( restcount < 6 )
            {
                target[ targetposition++ ] = made + 0x3c;
            }
            else
            {
                if ( targetposition < targetlength - 1 )
                {
                    target[ targetposition++ ] = made + 0x3c;
                    target[ targetposition++ ] = rest + 0x3c;
                }
                else
                {
                    target[ targetposition++ ] = made + 0x3c;
                }

                restcount = 0;
                rest = 0;
            }
        }

        if ( restcount > 0 )
        {
            target[ targetposition++ ] = rest + 0x3c;
        }

        target[ targetposition ] = '\0';
        return targetposition;
    }

    uint32 KFDecode::StringToUByte( const int8* source, uint32 sourcelength, uint8* target, uint32 targetlength )
    {
        uint32 targetposition = 0;
        uint32 bitposition = 2;
        uint32 madebit = 0;
        uint8 chars = 0;
        uint8 code = 0;
        uint8 temp = 0;

        for ( uint32 i = 0; i < sourcelength; ++i )
        {
            if ( ( ( uint8 )source[ i ] - 0x3c ) >= 0 )
            {
                chars = ( uint8 )source[ i ] - 0x3c;
            }
            else
            {
                targetposition = 0;
                break;
            }

            if ( targetposition >= targetlength )
            {
                break;
            }

            if ( ( madebit + 6 ) >= 8 )
            {
                code = ( temp | ( ( chars & 0x3f ) >> ( 6 - bitposition ) ) );
                target[ targetposition++ ] = code;

                madebit = 0;

                if ( bitposition < 6 )
                {
                    bitposition += 2;
                }
                else
                {
                    bitposition = 2;
                    continue;
                }
            }

            temp = ( ( chars << bitposition ) & _decode_bit_mask[ bitposition - 2 ] );
            madebit += ( 8 - bitposition );
        }
        return targetposition;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFDecode::UByteToString( const std::string& source )
    {
        static uint32 _length = KFBufferEnum::Buff_40M;
        static auto _buffer = __KF_INT8__( _length );

        // 转为可视字符串
        auto length = UByteToString( reinterpret_cast<const uint8*>( source.data() ), static_cast<uint32>( source.size() ), _buffer, _length );

        std::string result;
        result.assign( _buffer, length );
        return result;
    }

    std::string KFDecode::StringToUByte( const std::string& source )
    {
        static uint32 _length = KFBufferEnum::Buff_20M;
        static auto _buffer = __KF_UINT8__( _length );

        // 转为可视字符串
        auto length = StringToUByte( source.data(), static_cast<uint32>( source.size() ), _buffer, _length );

        std::string result;
        result.assign( reinterpret_cast<const int8*>( _buffer ), length );
        return result;
    }
}