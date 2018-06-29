#include "KFHttpCommon.h"
#include "KFJson.h"
#include "KFEnum.h"
#include "KFrame.h"
#include "KFUtility/KFUtility.h"
#include "KFCrypto/KFCrypto.h"

namespace KFrame
{
    static const char* _crypto_key = "kframe,./";

    void KFHttpCommon::MakeSignature( KFJson& json )
    {
        auto data = json.Serialize();
        auto temp = KFUtility::Format( "%s:%s", data.c_str(), _crypto_key );
        auto md4sign = KFCrypto::Md5Encode( temp );
        json.SetValue< const std::string& >( KFField::_sign, md4sign );
    }

    bool KFHttpCommon::VerifySignature( KFJson& json )
    {
        auto sign = json.GetString( KFField::_sign );
        json.removeMember( KFField::_sign );

        auto data = json.Serialize();


        auto temp = KFUtility::Format( "%s:%s", data.c_str(), _crypto_key );
        auto md4sign = KFCrypto::Md5Encode( temp );
        return sign == md4sign;
    }

    std::string KFHttpCommon::SendResponseCode( uint32 code )
    {
        KFJson response;
        response.SetValue( KFField::_ret_code, code );
        return SendResponse( response );
    }

    uint32 KFHttpCommon::GetResponseCode( KFJson& json )
    {
        return json.GetUInt32( KFField::_ret_code );
    }

    std::string KFHttpCommon::SendResponse( KFJson& json )
    {
        if ( !json.isMember( KFField::_ret_code ) )
        {
            json.SetValue< uint32 >( KFField::_ret_code, KFCommonEnum::OK );
        }

        return json.Serialize();
    }

    std::string KFHttpCommon::SendResponse( KFJson& json, uint32 code )
    {
        json.SetValue( KFField::_ret_code, code );
        return json.Serialize();
    }
}

