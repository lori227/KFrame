#ifndef __KF_CRYPTO_H__
#define __KF_CRYPTO_H__

#include "KFInclude.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    class KFCrypto
    {
    public:

        // md5
        static std::string Md5Encode( const std::string& source );
        static std::string Md5Encode( const char* data, uint32 length );

        // file
        static std::string Md5File( const std::string& file );

        // sha1
        static std::string Sha1Encode( const std::string& source );
        static std::string Sha1Encode( const char* data, uint32 length );

    };
}

#endif
