#include "KFHttpCommon.h"
#include "KFEnum.h"
#include "KFrame.h"

namespace KFrame
{
    static std::string _crypto_key = "kframe,./";

    void KFHttpCommon::MakeSignature( KFJson& json )
    {
        auto data = __JSON_SERIALIZE__( json );
        auto temp = __FORMAT__( "{}:{}", data, _crypto_key );
        auto md5sign = KFCrypto::Md5Encode( temp );

        json.AddMember( rapidjson::StringRef( __KF_STRING__( sign ).c_str() ), md5sign, json.GetAllocator() );
    }

    bool KFHttpCommon::VerifySignature( KFJson& json )
    {
        auto sign = __JSON_GET_STRING__( json, __KF_STRING__( sign ) );
        __JSON_REMOVE__( json, __KF_STRING__( sign ) );

        auto data = __JSON_SERIALIZE__( json );
        auto temp = __FORMAT__( "{}:{}", data, _crypto_key );
        auto md4sign = KFCrypto::Md5Encode( temp );
        return sign == md4sign;
    }

    std::string KFHttpCommon::SendResponseCode( uint32 code )
    {
        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( retcode ), code );
        return SendResponse( response );
    }

    uint32 KFHttpCommon::GetResponseCode( KFJson& json )
    {
        if ( !__JSON_HAS_MEMBER__( json, __KF_STRING__( retcode ) ) )
        {
            return _invalid_int;
        }

        return __JSON_GET_UINT32__( json, __KF_STRING__( retcode ) );
    }

    std::string KFHttpCommon::SendResponse( KFJson& json )
    {
        if ( !__JSON_HAS_MEMBER__( json, __KF_STRING__( retcode ) ) )
        {
            json.AddMember( rapidjson::StringRef( __KF_STRING__( retcode ).c_str() ), KFEnum::Ok, json.GetAllocator() );
        }

        return __JSON_SERIALIZE__( json );
    }

    std::string KFHttpCommon::SendResponse( KFJson& json, uint32 code )
    {
        json.AddMember( rapidjson::StringRef( __KF_STRING__( retcode ).c_str() ), code, json.GetAllocator() );
        return __JSON_SERIALIZE__( json );
    }
}

