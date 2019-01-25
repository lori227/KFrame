#include "KFCrypto.h"
#include "Poco/Crypto/DigestEngine.h"

namespace KFrame
{
    using Poco::Crypto::DigestEngine;

    std::string KFCrypto::Md5Encode( const std::string& source )
    {
        DigestEngine engine( "MD5" );
        engine.update( source );

        return DigestEngine::digestToHex( engine.digest() );
    }

    std::string KFCrypto::Md5Encode( const char* data, uint32 length )
    {
        DigestEngine engine( "MD5" );
        engine.update( data, length );

        return DigestEngine::digestToHex( engine.digest() );
    }
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    std::string KFCrypto::Sha1Encode( const std::string& source )
    {
        DigestEngine engine( "SHA1" );
        engine.update( source );

        return DigestEngine::digestToHex( engine.digest() );
    }

    std::string KFCrypto::Sha1Encode( const char* data, uint32 length )
    {
        DigestEngine engine( "SHA1" );
        engine.update( data, length );

        return DigestEngine::digestToHex( engine.digest() );
    }


}