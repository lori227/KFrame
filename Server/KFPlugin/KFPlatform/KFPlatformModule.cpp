#include "KFPlatformModule.h"
#include "KFHttp/KFHttpURL.h"
#include "KFPlatformConfig.h"

namespace KFrame
{
    void KFPlatformModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_platform_config, true );
    }

    void KFPlatformModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_platform_config );
    }

    const std::string& KFPlatformModule::GetPlatformApiUrl()
    {
        return _kf_platform_config->_app_url;
    }

    const std::string& KFPlatformModule::MakePlatformSign( uint32 nowtime )
    {
        static std::string _sign = "";

        auto source = __FORMAT__( "{}{}", _kf_platform_config->_app_key, nowtime );
        _sign = KFCrypto::Md5Encode( source );

        return _sign;
    }

    const std::string& KFPlatformModule::MakePlatformUrl( const std::string& path )
    {
        MapString params;
        params[ "appid" ] = _kf_platform_config->_app_id;
        params[ "timestamp" ] = KFUtility::ToString( KFGlobal::Instance()->_real_time );
        params[ "sig" ] = MakePlatformSign( KFGlobal::Instance()->_real_time );

        static std::string _total_url = "";
        _total_url = _kf_platform_config->_app_url + path + KFHttpURL::EncodeParams( params );
        return _total_url;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

}