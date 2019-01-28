#include "KFWeiXin.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
#define __LOGIN_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( account ) )
#define __Execute_Redis__( query ) \
    if ( !query ) \
    { \
        return _kf_http_server->SendCode( KFMsg::LoginDatabaseError );\
    }

    std::string KFWeiXin::RequestLogin( KFJson& json, const KFChannelSetting* kfsetting )
    {
        auto machinecode = __JSON_GET_STRING__( json, __KF_STRING__( machine ) );
        auto weixincode = __JSON_GET_STRING__( json, __KF_STRING__( code ) );

        std::string accesstoken = "";
        std::string openid = "";

        if ( !weixincode.empty() )
        {
            // 去请求access_token
            auto urldata = __FORMAT__( "{}/sns/oauth2/access_token?appid={}&secret={}&code={}&grant_type=authorization_code",
                                       kfsetting->_login_url, kfsetting->_app_id, kfsetting->_app_key, weixincode );

            auto accessdata = _kf_http_client->STGet( urldata, _invalid_str );
            if ( accessdata.empty() )
            {
                return _kf_http_server->SendCode( KFMsg::WeiXinError );
            }

            __JSON_PARSE_STRING__( accessjson, accessdata );

            // 如果出错, 返回错误码
            if ( __JSON_HAS_MEMBER__( accessjson, __KF_STRING__( errcode ) ) )
            {
                return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinCodeError );
            }

            accesstoken = __JSON_GET_STRING__( accessjson, __KF_STRING__( access_token ) );
            openid = __JSON_GET_STRING__( accessjson, __KF_STRING__( openid ) );
            auto refreshtoken = __JSON_GET_STRING__( accessjson, __KF_STRING__( refresh_token ) );
            auto expirestime = __JSON_GET_UINT32__( accessjson, __KF_STRING__( expires_in ) );
            auto scope = __JSON_GET_STRING__( accessjson, __KF_STRING__( scope ) );

            // 保存access_token
            auto redisdriver = __LOGIN_REDIS_DRIVER__;
            redisdriver->Append( "hmset {}:{} {} {} {} {} {} {}",
                                 __KF_STRING__( access_token ), machinecode, __KF_STRING__( access_token ), accesstoken,
                                 __KF_STRING__( openid ), openid, __KF_STRING__( scope ), scope );
            redisdriver->Append( "expire {}:{} {}", __KF_STRING__( access_token ), machinecode, expirestime - 200 );
            redisdriver->Append( "hset {}:{} {} {}", __KF_STRING__( refresh_token ), machinecode, __KF_STRING__( refresh_token ), accesstoken );
            redisdriver->Append( "expire {}:{} {}", __KF_STRING__( refresh_token ), machinecode, 2590000 );
            redisdriver->Pipeline();
        }
        else
        {
            // 机器码获得账号的access_token
            auto redisdriver = __LOGIN_REDIS_DRIVER__;
            auto weixindata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( access_token ), machinecode );
            if ( !weixindata->IsOk() )
            {
                return _kf_http_server->SendCode( KFMsg::AuthDatabaseBusy );
            }

            if ( !weixindata->_value.empty() )
            {
                openid = weixindata->_value[ __KF_STRING__( openid ) ];
                accesstoken = weixindata->_value[ __KF_STRING__( access_token ) ];
            }
            else
            {
                // 获得refresh_token
                auto refreshtoken = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( refresh_token ), machinecode, __KF_STRING__( refresh_token ) );
                if ( !refreshtoken->IsOk() )
                {
                    return _kf_http_server->SendCode( KFMsg::AuthDatabaseBusy );
                }

                if ( refreshtoken->_value.empty() )
                {
                    return _kf_http_server->SendCode( KFMsg::WeiXinTokenTimeout );
                }

                // 刷新access_token
                auto urldata = __FORMAT__( "{}/sns/oauth2/refresh_token?grant_type=refresh_token&appid={}&refresh_token={}",
                                           kfsetting->_login_url, kfsetting->_app_id, refreshtoken->_value );

                auto accessdata = _kf_http_client->STGet( urldata, _invalid_str );
                if ( accessdata.empty() )
                {
                    return _kf_http_server->SendCode( KFMsg::WeiXinError );
                }

                __JSON_PARSE_STRING__( accessjson, accessdata );
                if ( __JSON_HAS_MEMBER__( accessjson, __KF_STRING__( errcode ) ) )
                {
                    return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinTokenError );
                }

                accesstoken = __JSON_GET_STRING__( accessjson, __KF_STRING__( access_token ) );
                openid = __JSON_GET_STRING__( accessjson, __KF_STRING__( openid ) );
                auto expirestime = __JSON_GET_UINT32__( accessjson, __KF_STRING__( expires_in ) );
                auto scope = __JSON_GET_STRING__( accessjson, __KF_STRING__( scope ) );

                // 保存access_token
                redisdriver->Append( "hmset {}:{} {} {}", __KF_STRING__( access_token ), machinecode, __KF_STRING__( access_token ), accesstoken );
                redisdriver->Append( "expire {}:{} {}", __KF_STRING__( access_token ), machinecode, expirestime - 200 );
                redisdriver->Pipeline();
            }
        }

        // 获取用户资料
        auto urldata = __FORMAT__( "{}/sns/userinfo?access_token={}&openid={}", kfsetting->_login_url, accesstoken, openid );
        auto userdata = _kf_http_client->STGet( urldata, _invalid_str );
        if ( userdata.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::WeiXinError );
        }

        __JSON_PARSE_STRING__( userjson, userdata );
        if ( __JSON_HAS_MEMBER__( userjson, __KF_STRING__( errcode ) ) )
        {
            return _kf_http_server->SendResponse( userjson, KFMsg::WeiXinUserError );
        }

        //auto openid = __JSON_GET_STRING__( userjson, __KF_STRING__( openid ) );

        __JSON_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( account ), openid );
        __JSON_SET_VALUE__( response, __KF_STRING__( channel ), kfsetting->_channel_id );

        //KFJson kfextend;
        //kfextend.SetValue( __KF_STRING__( name ), userjson.GetString( __KF_STRING__( nickname ) ) );
        //kfextend.SetValue( __KF_STRING__( sex ), userjson.GetInt32( __KF_STRING__( sex ) ) );
        //kfextend.SetValue( __KF_STRING__( icon ), userjson.GetString( __KF_STRING__( headimgurl ) ) );
        //response.SetValue( __KF_STRING__( extend ), kfextend );

        return _kf_http_server->SendResponse( response );
    }

    std::string KFWeiXin::RequestPay( KFJson& json, const KFChannelSetting* kfsetting )
    {
        return _invalid_str;
    }
}