#include "KFInternal.h"
#include "KFJson.h"

namespace KFrame
{
    std::string KFInternal::RequestLogin( KFJson& json, const KFChannelSetting* kfchannelsetting )
    {
        auto account = json.GetString( __KF_STRING__( account ) );

        // 测试服直接登录
        KFJson response;
        response.SetValue( __KF_STRING__( account ), account );
        response.SetValue( __KF_STRING__( channel ), kfchannelsetting->_channel_id );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFInternal::RequestPay( KFJson& json, const KFChannelSetting* kfchannelsetting )
    {
        return _invalid_str;
    }
}