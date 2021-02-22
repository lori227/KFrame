#ifndef __KF_CONVERT_H__
#define __KF_CONVERT_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFConvert
    {
    public:

        static std::string ToUTF8( const std::string& source );
        static std::string ToAscii( const std::string& source );

    protected:
        static std::string ToCovert( void* iconv_handle, const std::string& source );
    };
}


#endif // __KF_CONVERT_H__
