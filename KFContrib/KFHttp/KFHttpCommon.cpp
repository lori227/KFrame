#include "KFHttpCommon.h"
#include "KFEnum.h"
#include "KFrame.h"

namespace KFrame
{
    static std::string _sign = "sign";
    static std::string _ret_code = "retcode";
    static std::string _crypto_key = "kframe,./";


    void KFHttpCommon::MakeSignature( KFJson& json )
    {
        auto data = __JSON_SERIALIZE__( json );
        auto temp = __FORMAT__( "{}:{}", data, _crypto_key );
        auto md5sign = KFCrypto::Md5Encode( temp );

        json.AddMember( rapidjson::StringRef( _sign.c_str() ), md5sign, json.GetAllocator() );
    }

    bool KFHttpCommon::VerifySignature( KFJson& json )
    {
        auto sign = __JSON_GET_STRING__( json, _sign );
        __JOSN_REMOVE__( json, _sign );

        auto data = __JSON_SERIALIZE__( json );
        auto temp = __FORMAT__( "{}:{}", data, _crypto_key );
        auto md4sign = KFCrypto::Md5Encode( temp );
        return sign == md4sign;
    }

    std::string KFHttpCommon::SendResponseCode( uint32 code )
    {
        __JSON_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, _ret_code, code );
        return SendResponse( response );
    }

    uint32 KFHttpCommon::GetResponseCode( KFJson& json )
    {
        if ( !__JSON_HAS_MEMBER__( json, _ret_code ) )
        {
            return _invalid_int;
        }

        return __JSON_GET_UINT32__( json, _ret_code );
    }

    std::string KFHttpCommon::SendResponse( KFJson& json )
    {
        if ( !__JSON_HAS_MEMBER__( json, _ret_code ) )
        {
            json.AddMember( rapidjson::StringRef( _ret_code.c_str() ), KFEnum::Ok, json.GetAllocator() );
        }

        return __JSON_SERIALIZE__( json );
    }

    std::string KFHttpCommon::SendResponse( KFJson& json, uint32 code )
    {
        json.AddMember( rapidjson::StringRef( _ret_code.c_str() ), code, json.GetAllocator() );
        return __JSON_SERIALIZE__( json );
    }
}

