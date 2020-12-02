#ifndef __MD5_H__
#define __MD5_H__

#include "KFInclude.h"

namespace KFrame
{
    class Md5
    {
    public:
        static std::string Md5Data( const std::string& data );
        static std::string Md5Data( const void* data, uint32 length );

        static std::string Md5File( const std::string& filename );

        static std::string md5Sum6( const std::string& data );
        static std::string md5Sum6( const void* data, uint32 length );
    };

}

#endif // end of __MD5_H__
