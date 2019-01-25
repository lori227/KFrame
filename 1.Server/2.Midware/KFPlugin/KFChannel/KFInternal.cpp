#include "KFInternal.h"

namespace KFrame
{
    std::string KFInternal::RequestLogin( KFJson& json, const KFChannelSetting* kfchannelsetting )
    {
        auto account = __JSON_GET_STRING__( json, __KF_STRING__( account ) );

        // 测试服直接登录
        __JSON_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( account ), account );
        __JSON_SET_VALUE__( response, __KF_STRING__( channel ), kfchannelsetting->_channel_id );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFInternal::RequestPay( KFJson& json, const KFChannelSetting* kfchannelsetting )
    {
        return _invalid_str;
    }
}