#include "KFPlatformModule.h"
#include "KFHttp/KFHttpURL.h"

namespace KFrame
{
    const std::string& KFPlatformModule::GetPlatformApiUrl()
    {
        static auto kfoption = _kf_option->FindOption( __KF_STRING__( platapiurl ) );
        return kfoption->_str_value;
    }

    const std::string& KFPlatformModule::MakePlatformSign( uint32 nowtime )
    {
        static std::string _sign = "";
        static auto _app_key = _kf_option->GetString( __KF_STRING__( platappkey ) );

        auto source = __FORMAT__( "{}{}", _app_key, nowtime );
        _sign = KFCrypto::Md5Encode( source );

        return _sign;
    }

    const std::string& KFPlatformModule::MakePlatformUrl( const std::string& path )
    {
        static auto _api_url = GetPlatformApiUrl();
        static auto _app_id = _kf_option->GetString( __KF_STRING__( platappid ) );

        MapString params;
        params[ "appid" ] = _app_id;
        params[ "timestamp" ] = KFUtility::ToString( KFGlobal::Instance()->_real_time );
        params[ "sig" ] = MakePlatformSign( KFGlobal::Instance()->_real_time );

        static std::string _total_url = "";
        _total_url = _api_url + path + KFHttpURL::EncodeParams( params );
        return _total_url;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

}