#ifndef __KF_CONVERT_H__
#define __KF_CONVERT_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFConvert
    {
    public:

        static std::string ToUTF8( const std::string& source );
        static std::string ToAscii( const std::string& source );

    protected:
        static std::string ToCovert( void* iconvhandle, const std::string& source );
    };
}


#endif // __KF_CONVERT_H__
