#include "KFDecode.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    static uint8 _decode_bit_mask[ 5 ] = { 0xfc, 0xf8, 0xf0, 0xe0, 0xc0 };

    uint32 KFDecode::UByteToString( const uint8* source, uint32 source_length, int8* target, uint32 target_length )
    {
        uint8 made = 0;
        uint8 rest = 0;
        uint32 target_position = 0;
        uint32 rest_count = 0;

        for ( uint32 i = 0; i < source_length; ++i )
        {
            if ( target_position >= target_length )
            {
                break;
            }

            made = ( ( rest | ( source[ i ] >> ( 2 + rest_count ) ) ) & 0x3f );
            rest = ( ( ( source[ i ] << ( 8 - ( 2 + rest_count ) ) ) >> 2 ) & 0x3f );

            rest_count += 2;

            if ( rest_count < 6 )
            {
                target[ target_position++ ] = made + 0x3c;
            }
            else
            {
                if ( target_position < target_length - 1 )
                {
                    target[ target_position++ ] = made + 0x3c;
                    target[ target_position++ ] = rest + 0x3c;
                }
                else
                {
                    target[ target_position++ ] = made + 0x3c;
                }

                rest_count = 0;
                rest = 0;
            }
        }

        if ( rest_count > 0 )
        {
            target[ target_position++ ] = rest + 0x3c;
        }

        target[ target_position ] = '\0';
        return target_position;
    }

    uint32 KFDecode::StringToUByte( const int8* source, uint32 source_length, uint8* target, uint32 target_length )
    {
        uint32 target_position = 0;
        uint32 bit_position = 2;
        uint32 made_bit = 0;
        uint8 chars = 0;
        uint8 code = 0;
        uint8 temp = 0;

        for ( uint32 i = 0; i < source_length; ++i )
        {
            if ( ( ( uint8 )source[ i ] - 0x3c ) >= 0 )
            {
                chars = ( uint8 )source[ i ] - 0x3c;
            }
            else
            {
                target_position = 0;
                break;
            }

            if ( target_position >= target_length )
            {
                break;
            }

            if ( ( made_bit + 6 ) >= 8 )
            {
                code = ( temp | ( ( chars & 0x3f ) >> ( 6 - bit_position ) ) );
                target[ target_position++ ] = code;

                made_bit = 0;

                if ( bit_position < 6 )
                {
                    bit_position += 2;
                }
                else
                {
                    bit_position = 2;
                    continue;
                }
            }

            temp = ( ( chars << bit_position ) & _decode_bit_mask[ bit_position - 2 ] );
            made_bit += ( 8 - bit_position );
        }
        return target_position;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFDecode::UByteToString( const std::string& source )
    {
        static uint32 _length = KFBufferEnum::Buff_40M;
        auto _buffer = __KF_INT8__( _length );

        // 转为可视字符串
        auto length = UByteToString( reinterpret_cast<const uint8*>( source.data() ), static_cast<uint32>( source.size() ), _buffer, _length );

        std::string result;
        result.assign( _buffer, length );
        return result;
    }

    std::string KFDecode::StringToUByte( const std::string& source )
    {
        static uint32 _length = KFBufferEnum::Buff_20M;
        auto _buffer = __KF_UINT8__( _length );

        // 转为可视字符串
        auto length = StringToUByte( source.data(), static_cast<uint32>( source.size() ), _buffer, _length );

        std::string result;
        result.assign( reinterpret_cast<const int8*>( _buffer ), length );
        return result;
    }
}