#ifndef __COMPRESS_STRING_H__
#define __COMPRESS_STRING_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFCompress
    {
    public:

        // 压缩
        static int32 Zib( const std::string& value, std::string& result );
        static int32 Compress( const std::string& value, std::string& result );

        // 解压缩
        static int32 UnZib( const std::string& value, std::string& result );
        static int32 UnCompress( const std::string& value, std::string& result );
    };
}

#endif // __COMPRESS_STRING_H__
