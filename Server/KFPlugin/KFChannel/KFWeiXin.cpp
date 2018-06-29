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
#define __LOGIN_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_login )
#define __Execute_Redis__( query ) \
    if ( !query ) \
    { \
        return _kf_http_server->SendResponseCode( KFMsg::LoginDatabaseError );\
    }

    std::string KFWeiXin::RequestLogin( KFJson& json, const KFChannelSetting* kfsetting )
    {
        auto machinecode = json.GetString( KFField::_machine );
        auto weixincode = json.GetString( KFField::_code );

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
            if ( accessjson.isMember( KFField::_err_code ) )
            {
                return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinCodeError );
            }

            accesstoken = accessjson.GetString( KFField::_access_token );
            openid = accessjson.GetString( KFField::_open_id );
            auto refreshtoken = accessjson.GetString( KFField::_refresh_token );
            auto expirestime = accessjson.GetUInt32( KFField::_expires_in );
            auto scope = accessjson.GetString( KFField::_scope );

            auto redisdriver = __LOGIN_REDIS_DRIVER__;

            // 保存access_token
            redisdriver->AppendCommand( "hmset %s:%s %s %s %s %s %s %s",
                                        KFField::_access_token.c_str(), machinecode.c_str(),
                                        KFField::_access_token.c_str(), accesstoken.c_str(),
                                        KFField::_open_id.c_str(), openid.c_str(),
                                        KFField::_scope.c_str(), scope.c_str() );
            redisdriver->AppendCommand( "expire %s:%s %u", KFField::_access_token.c_str(), machinecode.c_str(), expirestime - 200 );

            // 保存refresh_token
            redisdriver->AppendCommand( "hset %s:%s %s %s", KFField::_refresh_token.c_str(), machinecode.c_str(), KFField::_refresh_token.c_str(), accesstoken.c_str() );
            redisdriver->AppendCommand( "expire %s:%s %u", KFField::_refresh_token.c_str(), machinecode.c_str(), 2590000 );
            redisdriver->PipelineExecute();
        }
        else
        {
            // 机器码获得账号的access_token
            auto redisdriver = __LOGIN_REDIS_DRIVER__;
            MapString accessdata;
            __Execute_Redis__( redisdriver->MapExecute( accessdata, "hgetall %s:%s", KFField::_access_token.c_str(), machinecode.c_str() ) );
            if ( !accessdata.empty() )
            {
                openid = accessdata[ KFField::_open_id ];
                accesstoken = accessdata[ KFField::_access_token ];
            }
            else
            {
                // 获得refresh_token
                std::string refreshtoken = "";
                __Execute_Redis__( redisdriver->StringExecute( refreshtoken, "hget %s:%s %s", KFField::_refresh_token.c_str(), machinecode.c_str(), KFField::_refresh_token.c_str() ) );
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
                if ( accessjson.isMember( KFField::_err_code ) )
                {
                    return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinTokenError );
                }

                accesstoken = accessjson.GetString( KFField::_access_token );
                openid = accessjson.GetString( KFField::_open_id );
                auto expirestime = accessjson.GetUInt32( KFField::_expires_in );
                auto scope = accessjson.GetString( KFField::_scope );

                // 保存access_token
                redisdriver->AppendCommand( "hmset %s:%s %s %s", KFField::_access_token.c_str(), machinecode.c_str(), KFField::_access_token.c_str(), accesstoken.c_str() );
                redisdriver->AppendCommand( "expire %s:%s %u", KFField::_access_token.c_str(), machinecode.c_str(), expirestime - 200 );
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
        if ( userjson.isMember( KFField::_err_code ) )
        {
            return _kf_http_server->SendResponse( userjson, KFMsg::WeiXinUserError );
        }

        KFJson response;
        response.SetValue( KFField::_channel, _channel );
        response.SetValue( KFField::_app_id, kfsetting->_app_id );
        response.SetValue( KFField::_app_key, kfsetting->_app_key );
        response.SetValue( KFField::_account, userjson.GetString( KFField::_union_id ) );
        response.SetValue( KFField::_name, userjson.GetString( KFField::_nick_name ) );
        response.SetValue( KFField::_sex, userjson.GetInt32( KFField::_sex ) );
        response.SetValue( KFField::_icon, userjson.GetString( KFField::_head_img_url ) );
        return _kf_http_server->SendResponse( response );
    }

    std::string KFWeiXin::RequestPay( KFJson& json, const KFChannelSetting* kfchannelsetting )
    {
        return _invalid_str;
    }
}