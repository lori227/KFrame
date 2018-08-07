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

        // sha1
        static std::string Sha1Encode( const std::string& source );

    };
}

#endif
