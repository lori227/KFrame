#include "KFSteam.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    std::string KFSteam::RequestLogin( KFJson& json, const KFChannelSetting* kfsetting )
    {
        auto token = __JSON_GET_STRING__( json, __KF_STRING__( token ) );

        auto urldata = __FORMAT__( "{}?key={}&appid={}&ticket={}", kfsetting->_login_url, kfsetting->_app_key, kfsetting->_app_id, token );

        auto resultdata = _kf_http_client->StartSTHttpClient( urldata, _invalid_str );
        if ( resultdata.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamError );
        }

        __JSON_PARSE_STRING__( kfresult, resultdata );
        if ( !__JSON_HAS_MEMBER__( kfresult, __KF_STRING__( response ) ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamDataError );
        }

        // {"response":{"error":{"errorcode":101,"errordesc":"Invalid ticket"}}}
        auto& kfdata = kfresult[ __KF_STRING__( response ) ];
        if ( __JSON_HAS_MEMBER__( kfdata, __KF_STRING__( error ) ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamAuthError );
        }

        //{"response":{"params":{"result":"OK","steamid":"76561198859198521","ownersteamid":"76561198859198521","vacbanned":false,"publisherbanned":false}}}
        if ( !__JSON_HAS_MEMBER__( kfdata, __KF_STRING__( params ) ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamDataError );
        }

        auto& kfparams = kfdata[ __KF_STRING__( params ) ];
        if ( !__JSON_HAS_MEMBER__( kfparams, __KF_STRING__( steamid ) ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamDataError );
        }

        auto steamid = __JSON_GET_STRING__( kfparams, __KF_STRING__( steamid ) );

        __JSON_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( account ), steamid );
        __JSON_SET_VALUE__( response, __KF_STRING__( channel ), kfsetting->_channel_id );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFSteam::RequestPay( KFJson& json, const KFChannelSetting* kfsetting )
    {
        return _invalid_str;
    }
}