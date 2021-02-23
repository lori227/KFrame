#include "KFSteam.hpp"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    std::string KFSteam::RequestLogin( KFJson& json, const KFChannelSetting* setting )
    {
        auto token = __JSON_GET_STRING__( json, __STRING__( token ) );

        auto urldata = __FORMAT__( "{}?key={}&appid={}&ticket={}", setting->_login_url, setting->_app_key, setting->_app_id, token );

        auto resultdata = _kf_http_client->STGet( urldata, _invalid_string );
        if ( resultdata.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::SteamError );
        }

        __JSON_PARSE_STRING__( kfresult, resultdata );
        if ( !__JSON_HAS_MEMBER__( kfresult, __STRING__( response ) ) )
        {
            return _kf_http_server->SendCode( KFMsg::SteamDataError );
        }

        // {"response":{"error":{"errorcode":101,"errordesc":"Invalid ticket"}}}
        auto& kfdata = kfresult[ __STRING__( response ) ];
        if ( __JSON_HAS_MEMBER__( kfdata, __STRING__( error ) ) )
        {
            return _kf_http_server->SendCode( KFMsg::SteamAuthError );
        }

        //{"response":{"params":{"result":"OK","steamid":"76561198859198521","ownersteamid":"76561198859198521","vacbanned":false,"publisherbanned":false}}}
        if ( !__JSON_HAS_MEMBER__( kfdata, __STRING__( params ) ) )
        {
            return _kf_http_server->SendCode( KFMsg::SteamDataError );
        }

        auto& kfparams = kfdata[ __STRING__( params ) ];
        if ( !__JSON_HAS_MEMBER__( kfparams, __STRING__( steamid ) ) )
        {
            return _kf_http_server->SendCode( KFMsg::SteamDataError );
        }

        auto steamid = __JSON_GET_STRING__( kfparams, __STRING__( steamid ) );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( account ), steamid );
        __JSON_SET_VALUE__( response, __STRING__( channel ), setting->_id );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFSteam::RequestPay( const std::string& data, const KFChannelSetting* setting )
    {
        return _invalid_string;
    }
}