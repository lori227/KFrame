#include "KFWeiXin.hpp"
#include "KFProtocol/KFProtocol.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFAccount/KFAccountInterface.h"

namespace KFrame
{
    std::string KFWeiXin::RequestLogin( KFJson& json, const KFChannelSetting* setting )
    {
        auto machinecode = __JSON_GET_STRING__( json, __STRING__( machine ) );
        auto weixincode = __JSON_GET_STRING__( json, __STRING__( code ) );

        std::string openid = "";
        std::string accesstoken = "";

        if ( !weixincode.empty() )
        {
            // 去请求access_token
            auto urldata = __FORMAT__( "{}/sns/oauth2/access_token?appid={}&secret={}&code={}&grant_type=authorization_code",
                                       setting->_login_url, setting->_app_id, setting->_app_key, weixincode );

            auto accessdata = _kf_http_client->STGet( urldata, _invalid_string );
            if ( accessdata.empty() )
            {
                return _kf_http_server->SendCode( KFMsg::WeiXinError );
            }

            __JSON_PARSE_STRING__( accessjson, accessdata );
            if ( __JSON_HAS_MEMBER__( accessjson, __STRING__( errcode ) ) )
            {
                return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinCodeError );
            }

            openid = __JSON_GET_STRING__( accessjson, __STRING__( openid ) );
            accesstoken = __JSON_GET_STRING__( accessjson, __STRING__( access_token ) );

            // 保存访问token
            SaveAccessToken( machinecode, openid, accesstoken, accessjson );

            // 保存刷新token
            auto refreshtoken = __JSON_GET_STRING__( accessjson, __STRING__( refresh_token ) );
            _kf_account->SaveWeiXinRefreshToken( machinecode, refreshtoken );
        }
        else
        {
            // 机器码获得账号的access_token
            auto weixindata = _kf_account->QueryWeiXinAccessToken( machinecode );
            if ( !weixindata.empty() )
            {
                openid = weixindata[ __STRING__( openid ) ];
                accesstoken = weixindata[ __STRING__( token ) ];
            }
            else
            {
                // 获得refresh_token
                auto refreshtoken = _kf_account->QueryWeiXinRefreshToken( machinecode );
                if ( refreshtoken.empty() )
                {
                    return _kf_http_server->SendCode( KFMsg::WeiXinTokenTimeout );
                }

                // 刷新access_token
                auto urldata = __FORMAT__( "{}/sns/oauth2/refresh_token?grant_type=refresh_token&appid={}&refresh_token={}",
                                           setting->_login_url, setting->_app_id, refreshtoken );

                auto accessdata = _kf_http_client->STGet( urldata, _invalid_string );
                if ( accessdata.empty() )
                {
                    return _kf_http_server->SendCode( KFMsg::WeiXinError );
                }

                __JSON_PARSE_STRING__( accessjson, accessdata );
                if ( __JSON_HAS_MEMBER__( accessjson, __STRING__( errcode ) ) )
                {
                    return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinTokenError );
                }

                openid = __JSON_GET_STRING__( accessjson, __STRING__( openid ) );
                accesstoken = __JSON_GET_STRING__( accessjson, __STRING__( access_token ) );
                SaveAccessToken( machinecode, openid, accesstoken, accessjson );
            }
        }

        // 获取用户资料
        auto urldata = __FORMAT__( "{}/sns/userinfo?access_token={}&openid={}", setting->_login_url, accesstoken, openid );
        auto userdata = _kf_http_client->STGet( urldata, _invalid_string );
        if ( userdata.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::WeiXinError );
        }

        __JSON_PARSE_STRING__( userjson, userdata );
        if ( __JSON_HAS_MEMBER__( userjson, __STRING__( errcode ) ) )
        {
            return _kf_http_server->SendResponse( userjson, KFMsg::WeiXinUserError );
        }

        //auto openid = __JSON_GET_STRING__( userjson, __STRING__( openid ) );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( account ), openid );
        __JSON_SET_VALUE__( response, __STRING__( channel ), setting->_id );

        //KFJson kfextend;
        //kfextend.SetValue( __STRING__( name ), userjson.GetString( __STRING__( nickname ) ) );
        //kfextend.SetValue( __STRING__( sex ), userjson.GetInt32( __STRING__( sex ) ) );
        //kfextend.SetValue( __STRING__( icon ), userjson.GetString( __STRING__( headimgurl ) ) );
        //response.SetValue( __STRING__( extend ), kfextend );

        return _kf_http_server->SendResponse( response );
    }

    void KFWeiXin::SaveAccessToken( const std::string& machinecode, const std::string& openid, const std::string& accesstoken, KFJson& kfjson )
    {
        auto scope = __JSON_GET_STRING__( kfjson, __STRING__( scope ) );
        auto expirestime = __JSON_GET_UINT32__( kfjson, __STRING__( expires_in ) );
        _kf_account->SaveWeiXinAccessToken( machinecode, openid, scope, accesstoken, expirestime - 200 );
    }

    std::string KFWeiXin::RequestPay( const std::string& data, const KFChannelSetting* setting )
    {
        return _invalid_string;
    }
}