#include "KFSteam.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    std::string KFSteam::RequestLogin( KFJson& json, const KFChannelSetting* kfsetting )
    {
        auto token = json.GetString( __KF_STRING__( token ) );

        auto urldata = __FORMAT__( "{}?key={}&appid={}&ticket={}", kfsetting->_login_url, kfsetting->_app_key, kfsetting->_app_id, token );

        auto resultdata = _kf_http_client->StartSTHttpsClient( urldata, _invalid_str );
        if ( resultdata.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamError );
        }

        KFJson kfresult( resultdata );
        if ( !kfresult.isMember( __KF_STRING__( response ) ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamDataError );
        }

        // {"response":{"error":{"errorcode":101,"errordesc":"Invalid ticket"}}}
        KFJson kfdata = kfresult[ __KF_STRING__( response ) ];
        if ( kfdata.isMember( __KF_STRING__( error ) ) )
        {
            KFJson kferror = kfdata[ __KF_STRING__( error ) ];
            return _kf_http_server->SendResponse( kferror, KFMsg::SteamAuthError );
        }

        //{"response":{"params":{"result":"OK","steamid":"76561198859198521","ownersteamid":"76561198859198521","vacbanned":false,"publisherbanned":false}}}
        if ( !kfdata.isMember( __KF_STRING__( params ) ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamDataError );
        }

        KFJson kfparams = kfdata[ __KF_STRING__( params ) ];
        if ( !kfparams.isMember( __KF_STRING__( steamid ) ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SteamDataError );
        }

        auto steamid = kfparams.GetString( __KF_STRING__( steamid ) );

        KFJson response;
        response.SetValue( __KF_STRING__( account ), steamid );
        response.SetValue( __KF_STRING__( channel ), kfsetting->_channel_id );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFSteam::RequestPay( KFJson& json, const KFChannelSetting* kfsetting )
    {
        return _invalid_str;
    }
}