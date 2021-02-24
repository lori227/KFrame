#include "KFSteam.hpp"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    std::string KFSteam::RequestLogin( KFJson& json, std::shared_ptr<const KFChannelSetting> setting )
    {
        auto token = __JSON_GET_STRING__( json, __STRING__( token ) );

        auto url_data = __FORMAT__( "{}?key={}&appid={}&ticket={}", setting->_login_url, setting->_app_key, setting->_app_id, token );

        auto result_data = _kf_http_client->STGet( url_data, _invalid_string );
        if ( result_data.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::SteamError );
        }

        __JSON_PARSE_STRING__( result_json, result_data );
        if ( !__JSON_HAS_MEMBER__( result_json, __STRING__( response ) ) )
        {
            return _kf_http_server->SendCode( KFMsg::SteamDataError );
        }

        // {"response":{"error":{"errorcode":101,"errordesc":"Invalid ticket"}}}
        auto& login_result = result_json[ __STRING__( response ) ];
        if ( __JSON_HAS_MEMBER__( login_result, __STRING__( error ) ) )
        {
            return _kf_http_server->SendCode( KFMsg::SteamAuthError );
        }

        //{"response":{"params":{"result":"OK","steamid":"76561198859198521","ownersteamid":"76561198859198521","vacbanned":false,"publisherbanned":false}}}
        if ( !__JSON_HAS_MEMBER__( login_result, __STRING__( params ) ) )
        {
            return _kf_http_server->SendCode( KFMsg::SteamDataError );
        }

        auto& params = login_result[ __STRING__( params ) ];
        if ( !__JSON_HAS_MEMBER__( params, __STRING__( steamid ) ) )
        {
            return _kf_http_server->SendCode( KFMsg::SteamDataError );
        }

        auto steam_id = __JSON_GET_STRING__( params, __STRING__( steamid ) );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( account ), steam_id );
        __JSON_SET_VALUE__( response, __STRING__( channel ), setting->_id );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFSteam::RequestPay( const std::string& data, std::shared_ptr<const KFChannelSetting> setting )
    {
        return _invalid_string;
    }
}