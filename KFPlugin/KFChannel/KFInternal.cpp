#include "KFInternal.hpp"
#include "KFProtocol/KFProtocol.h"

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

    std::string KFInternal::RequestPay( const std::string& data, const KFChannelSetting* kfchannelsetting )
    {
        __JSON_PARSE_STRING__( request, data );

        auto order = __JSON_GET_STRING__( request, __KF_STRING__( order ) );

        MapString values;
        __JSON_TO_MAP__( request, values );

        auto ok = SavePayData( order, values );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }
}