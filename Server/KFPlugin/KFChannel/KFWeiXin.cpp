#include "KFWeiXin.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    KFWeiXin::KFWeiXin( uint32 channel ) : KFChannel( channel )
    {

    }

    KFWeiXin::~KFWeiXin()
    {

    }
#define __LOGIN_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( login ) )
#define __Execute_Redis__( query ) \
    if ( !query ) \
    { \
        return _kf_http_server->SendResponseCode( KFMsg::LoginDatabaseError );\
    }

    std::string KFWeiXin::RequestLogin( KFJson& json, const KFChannelSetting* kfsetting )
    {
        auto machinecode = json.GetString( __KF_CHAR__( machine ) );
        auto weixincode = json.GetString( __KF_CHAR__( code ) );

        std::string accesstoken = "";
        std::string openid = "";

        if ( !weixincode.empty() )
        {
            // 去请求access_token
            auto urldata = KFUtility::Format( "%s/sns/oauth2/access_token?appid=%s&secret=%s&code=%s&grant_type=authorization_code",
                                              kfsetting->_login_url.c_str(), kfsetting->_app_id.c_str(), kfsetting->_app_key.c_str(), weixincode.c_str() );

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
            redisdriver->AppendCommand( "hmset %s:%s %s %s %s %s %s %s",
                                        __KF_CHAR__( accesstoken ), machinecode.c_str(),
                                        __KF_CHAR__( accesstoken ), accesstoken.c_str(),
                                        __KF_CHAR__( openid ), openid.c_str(),
                                        __KF_CHAR__( scope ), scope.c_str() );
            redisdriver->AppendCommand( "expire %s:%s %u", __KF_CHAR__( accesstoken ), machinecode.c_str(), expirestime - 200 );

            // 保存refresh_token
            redisdriver->AppendCommand( "hset %s:%s %s %s", __KF_CHAR__( refreshtoken ), machinecode.c_str(), __KF_CHAR__( refreshtoken ), accesstoken.c_str() );
            redisdriver->AppendCommand( "expire %s:%s %u", __KF_CHAR__( refreshtoken ), machinecode.c_str(), 2590000 );
            redisdriver->PipelineExecute();
        }
        else
        {
            // 机器码获得账号的access_token
            auto redisdriver = __LOGIN_REDIS_DRIVER__;
            MapString accessdata;
            __Execute_Redis__( redisdriver->MapExecute( accessdata, "hgetall %s:%s", __KF_CHAR__( accesstoken ), machinecode.c_str() ) );
            if ( !accessdata.empty() )
            {
                openid = accessdata[ __KF_STRING__( openid ) ];
                accesstoken = accessdata[ __KF_STRING__( accesstoken ) ];
            }
            else
            {
                // 获得refresh_token
                std::string refreshtoken = "";
                __Execute_Redis__( redisdriver->StringExecute( refreshtoken, "hget %s:%s %s", __KF_CHAR__( refreshtoken ), machinecode.c_str(), __KF_CHAR__( refreshtoken ) ) );
                if ( refreshtoken.empty() )
                {
                    return _kf_http_server->SendResponseCode( KFMsg::WeiXinTokenTimeout );
                }

                // 刷新access_token
                auto urldata = KFUtility::Format( "%s/sns/oauth2/refresh_token?grant_type=refresh_token&appid=%s&refresh_token=%s",
                                                  kfsetting->_login_url.c_str(), kfsetting->_app_id.c_str(), refreshtoken.c_str() );

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
                redisdriver->AppendCommand( "hmset %s:%s %s %s", __KF_CHAR__( accesstoken ), machinecode.c_str(), __KF_CHAR__( accesstoken ), accesstoken.c_str() );
                redisdriver->AppendCommand( "expire %s:%s %u", __KF_CHAR__( accesstoken ), machinecode.c_str(), expirestime - 200 );
                redisdriver->PipelineExecute();
            }
        }

        // 获取用户资料
        auto urldata = KFUtility::Format( "%s/sns/userinfo?access_token=%s&openid=%s",
                                          kfsetting->_login_url.c_str(), accesstoken.c_str(), openid.c_str() );

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
        response.SetValue( __KF_STRING__( channel ), _channel );
        response.SetValue( __KF_STRING__( appid ), kfsetting->_app_id );
        response.SetValue( __KF_STRING__( appkey ), kfsetting->_app_key );
        response.SetValue( __KF_STRING__( account ), userjson.GetString( __KF_STRING__( unionid ) ) );
        response.SetValue( __KF_STRING__( name ), userjson.GetString( __KF_STRING__( nickname ) ) );
        response.SetValue( __KF_STRING__( sex ), userjson.GetInt32( __KF_STRING__( sex ) ) );
        response.SetValue( __KF_STRING__( icon ), userjson.GetString( __KF_STRING__( headimgurl ) ) );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFWeiXin::RequestPay( KFJson& json, const KFChannelSetting* kfchannelsetting )
    {
        return _invalid_str;
    }
}