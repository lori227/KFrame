#ifndef __KF_DECODE_H__
#define __KF_DECODE_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFDecode
    {
    public:
        // 数据转成可视化字串
        static std::string UByteToString( const std::string& source );
        static uint32 UByteToString( const uint8* source, uint32 source_length, int8* target, uint32 target_length );

        // 可视化字串转成数据
        static std::string StringToUByte( const std::string& source );
        static uint32 StringToUByte( const int8* source, uint32 source_length, uint8* target, uint32 target_length );
    };
}


#endif // __COMPRESS_STRING_H__
