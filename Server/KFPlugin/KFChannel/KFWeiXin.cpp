#include "KFWeiXin.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
#define __LOGIN_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( login ) )
#define __Execute_Redis__( query ) \
    if ( !query ) \
    { \
        return _kf_http_server->SendResponseCode( KFMsg::LoginDatabaseError );\
    }

    std::string KFWeiXin::RequestLogin( KFJson& json, const KFChannelSetting* kfsetting )
    {
        auto machinecode = json.GetString( __KF_STRING__( machine ) );
        auto weixincode = json.GetString( __KF_STRING__( code ) );

        std::string accesstoken = "";
        std::string openid = "";

        if ( !weixincode.empty() )
        {
            // 去请求access_token
            auto urldata = __FORMAT__( "{}/sns/oauth2/access_token?appid={}&secret={}&code={}&grant_type=authorization_code",
                                       kfsetting->_login_url, kfsetting->_app_id, kfsetting->_app_key, weixincode );

            auto accessdata = _kf_http_client->StartSTHttpsClient( urldata, _invalid_str );
            if ( accessdata.empty() )
            {
                return _kf_http_server->SendResponseCode( KFMsg::WeiXinError );
            }

            KFJson accessjson( accessdata );

            // 如果出错, 返回错误码
            if ( accessjson.isMember( __KF_STRING__( errcode ) ) )
            {
                return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinCodeError );
            }

            accesstoken = accessjson.GetString( __KF_STRING__( accesstoken ) );
            openid = accessjson.GetString( __KF_STRING__( openid ) );
            auto refreshtoken = accessjson.GetString( __KF_STRING__( refreshtoken ) );
            auto expirestime = accessjson.GetUInt32( __KF_STRING__( expiresin ) );
            auto scope = accessjson.GetString( __KF_STRING__( scope ) );

            auto redisdriver = __LOGIN_REDIS_DRIVER__;

            // 保存access_token
            redisdriver->Append( "hmset {}:{} {} {} {} {} {} {}",
                                 __KF_STRING__( accesstoken ), machinecode,
                                 __KF_STRING__( accesstoken ), accesstoken,
                                 __KF_STRING__( openid ), openid,
                                 __KF_STRING__( scope ), scope );
            redisdriver->Append( "expire {}:{} {}", __KF_STRING__( accesstoken ), machinecode, expirestime - 200 );

            // 保存refresh_token
            redisdriver->Append( "hset {}:{} {} {}", __KF_STRING__( refreshtoken ), machinecode, __KF_STRING__( refreshtoken ), accesstoken );
            redisdriver->Append( "expire {}:{} {}", __KF_STRING__( refreshtoken ), machinecode, 2590000 );
            redisdriver->Pipeline();
        }
        else
        {
            // 机器码获得账号的access_token
            auto redisdriver = __LOGIN_REDIS_DRIVER__;

            auto weixindata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( accesstoken ), machinecode );
            if ( !weixindata->IsOk() )
            {
                return _kf_http_server->SendResponseCode( KFMsg::LoginDatabaseError );
            }

            if ( !weixindata->_value.empty() )
            {
                openid = weixindata->_value[ __KF_STRING__( openid ) ];
                accesstoken = weixindata->_value[ __KF_STRING__( accesstoken ) ];
            }
            else
            {
                // 获得refresh_token
                auto refreshtoken = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( refreshtoken ), machinecode, __KF_STRING__( refreshtoken ) );
                if ( !refreshtoken->IsOk() )
                {
                    return _kf_http_server->SendResponseCode( KFMsg::LoginDatabaseError );
                }

                if ( refreshtoken->_value.empty() )
                {
                    return _kf_http_server->SendResponseCode( KFMsg::WeiXinTokenTimeout );
                }

                // 刷新access_token
                auto urldata = __FORMAT__( "{}/sns/oauth2/refresh_token?grant_type=refresh_token&appid={}&refresh_token={}",
                                           kfsetting->_login_url, kfsetting->_app_id, refreshtoken->_value );

                auto accessdata = _kf_http_client->StartSTHttpsClient( urldata, _invalid_str );
                if ( accessdata.empty() )
                {
                    return _kf_http_server->SendResponseCode( KFMsg::WeiXinError );
                }

                KFJson accessjson( accessdata );
                if ( accessjson.isMember( __KF_STRING__( errcode ) ) )
                {
                    return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinTokenError );
                }

                accesstoken = accessjson.GetString( __KF_STRING__( accesstoken ) );
                openid = accessjson.GetString( __KF_STRING__( openid ) );
                auto expirestime = accessjson.GetUInt32( __KF_STRING__( expiresin ) );
                auto scope = accessjson.GetString( __KF_STRING__( scope ) );

                // 保存access_token
                redisdriver->Append( "hmset {}:{} {} {}", __KF_STRING__( accesstoken ), machinecode, __KF_STRING__( accesstoken ), accesstoken );
                redisdriver->Append( "expire {}:{} {}", __KF_STRING__( accesstoken ), machinecode, expirestime - 200 );
                redisdriver->Pipeline();
            }
        }

        // 获取用户资料
        auto urldata = __FORMAT__( "{}/sns/userinfo?access_token={}&openid={}", kfsetting->_login_url, accesstoken, openid );
        auto userdata = _kf_http_client->StartSTHttpsClient( urldata, _invalid_str );
        if ( userdata.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::WeiXinError );
        }

        KFJson userjson( userdata );
        if ( userjson.isMember( __KF_STRING__( errcode ) ) )
        {
            return _kf_http_server->SendResponse( userjson, KFMsg::WeiXinUserError );
        }

        KFJson response;
        response.SetValue( __KF_STRING__( channel ), kfsetting->_channel_id );
        response.SetValue( __KF_STRING__( account ), userjson.GetString( __KF_STRING__( unionid ) ) );
        response.SetValue( __KF_STRING__( name ), userjson.GetString( __KF_STRING__( nickname ) ) );
        response.SetValue( __KF_STRING__( sex ), userjson.GetInt32( __KF_STRING__( sex ) ) );
        response.SetValue( __KF_STRING__( icon ), userjson.GetString( __KF_STRING__( headimgurl ) ) );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFWeiXin::RequestPay( KFJson& json, const KFChannelSetting* kfsetting )
    {
        return _invalid_str;
    }
}