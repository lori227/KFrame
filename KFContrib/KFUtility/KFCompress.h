#ifndef __COMPRESS_STRING_H__
#define __COMPRESS_STRING_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFCompress
    {
    public:

        // 压缩
        static bool Compress( const std::string& value, std::string& result, uint32 compresstype, uint32 compresslevel, bool convert );

        // 解压缩
        static bool UnCompress( const std::string& value, std::string& result, uint32 compresstype, bool convert );

    protected:
    };
}

#endif // __COMPRESS_STRING_H__
