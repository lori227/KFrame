#include "KFInternal.h"
#include "KFJson.h"

namespace KFrame
{
    KFInternal::KFInternal( uint32 channel ) : KFChannel( channel )
    {

    }

    KFInternal::~KFInternal()
    {

    }

    std::string KFInternal::RequestLogin( KFJson& json, const KFChannelSetting* kfchannelsetting )
    {
        auto account = json.GetString( __KF_STRING__( account ) );

        // 测试服直接登录
        KFJson response;
        response.SetValue( __KF_STRING__( account ), account );
        response.SetValue( __KF_STRING__( channel ), _channel );
        response.SetValue( __KF_STRING__( appid ), kfchannelsetting->_app_id );
        response.SetValue( __KF_STRING__( appkey ), kfchannelsetting->_app_key );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFInternal::RequestPay( KFJson& json, const KFChannelSetting* kfchannelsetting )
    {
        return _invalid_str;
    }
}