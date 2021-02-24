#include "KFWeiXin.hpp"
#include "KFProtocol/KFProtocol.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFAccount/KFAccountInterface.h"

namespace KFrame
{
    std::string KFWeiXin::RequestLogin( KFJson& json, std::shared_ptr<const KFChannelSetting> setting )
    {
        auto machine_code = __JSON_GET_STRING__( json, __STRING__( machine ) );
        auto wei_xin_code = __JSON_GET_STRING__( json, __STRING__( code ) );

        std::string open_id = "";
        std::string access_token = "";

        if ( !wei_xin_code.empty() )
        {
            // 去请求access_token
            auto url_data = __FORMAT__( "{}/sns/oauth2/access_token?appid={}&secret={}&code={}&grant_type=authorization_code",
                                       setting->_login_url, setting->_app_id, setting->_app_key, wei_xin_code );

            auto access_data = _kf_http_client->STGet( url_data, _invalid_string );
            if ( access_data.empty() )
            {
                return _kf_http_server->SendCode( KFMsg::WeiXinError );
            }

            __JSON_PARSE_STRING__( access_json, access_data );
            if ( __JSON_HAS_MEMBER__( access_json, __STRING__( errcode ) ) )
            {
                return _kf_http_server->SendResponse( access_json, KFMsg::WeiXinCodeError );
            }

            open_id = __JSON_GET_STRING__( access_json, __STRING__( openid ) );
            access_token = __JSON_GET_STRING__( access_json, __STRING__( access_token ) );

            // 保存访问token
            SaveAccessToken( machine_code, open_id, access_token, access_json );

            // 保存刷新token
            auto refresh_token = __JSON_GET_STRING__( access_json, __STRING__( refresh_token ) );
            _kf_account->SaveWeiXinRefreshToken( machine_code, refresh_token );
        }
        else
        {
            // 机器码获得账号的access_token
            auto wei_xin_data = _kf_account->QueryWeiXinAccessToken( machine_code );
            if ( !wei_xin_data.empty() )
            {
                open_id = wei_xin_data[ __STRING__( openid ) ];
                access_token = wei_xin_data[ __STRING__( token ) ];
            }
            else
            {
                // 获得refresh_token
                auto refresh_token = _kf_account->QueryWeiXinRefreshToken( machine_code );
                if ( refresh_token.empty() )
                {
                    return _kf_http_server->SendCode( KFMsg::WeiXinTokenTimeout );
                }

                // 刷新access_token
                auto url_data = __FORMAT__( "{}/sns/oauth2/refresh_token?grant_type=refresh_token&appid={}&refresh_token={}",
                                           setting->_login_url, setting->_app_id, refresh_token );

                auto access_data = _kf_http_client->STGet( url_data, _invalid_string );
                if ( access_data.empty() )
                {
                    return _kf_http_server->SendCode( KFMsg::WeiXinError );
                }

                __JSON_PARSE_STRING__( accessjson, access_data );
                if ( __JSON_HAS_MEMBER__( accessjson, __STRING__( errcode ) ) )
                {
                    return _kf_http_server->SendResponse( accessjson, KFMsg::WeiXinTokenError );
                }

                open_id = __JSON_GET_STRING__( accessjson, __STRING__( openid ) );
                access_token = __JSON_GET_STRING__( accessjson, __STRING__( access_token ) );
                SaveAccessToken( machine_code, open_id, access_token, accessjson );
            }
        }

        // 获取用户资料
        auto url_data = __FORMAT__( "{}/sns/userinfo?access_token={}&openid={}", setting->_login_url, access_token, open_id );
        auto user_data = _kf_http_client->STGet( url_data, _invalid_string );
        if ( user_data.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::WeiXinError );
        }

        __JSON_PARSE_STRING__( user_json, user_data );
        if ( __JSON_HAS_MEMBER__( user_json, __STRING__( errcode ) ) )
        {
            return _kf_http_server->SendResponse( user_json, KFMsg::WeiXinUserError );
        }

        //auto openid = __JSON_GET_STRING__( user_json, __STRING__( openid ) );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( account ), open_id );
        __JSON_SET_VALUE__( response, __STRING__( channel ), setting->_id );

        //KFJson extend_data;
        //extend_data.SetValue( __STRING__( name ), user_json.GetString( __STRING__( nickname ) ) );
        //extend_data.SetValue( __STRING__( sex ), user_json.GetInt32( __STRING__( sex ) ) );
        //extend_data.SetValue( __STRING__( icon ), user_json.GetString( __STRING__( headimgurl ) ) );
        //response.SetValue( __STRING__( extend ), extend_data );

        return _kf_http_server->SendResponse( response );
    }

    void KFWeiXin::SaveAccessToken( const std::string& machine_code, const std::string& open_id, const std::string& access_token, KFJson& json_data )
    {
        auto scope = __JSON_GET_STRING__( json_data, __STRING__( scope ) );
        auto expire_time = __JSON_GET_UINT32__( json_data, __STRING__( expires_in ) );
        _kf_account->SaveWeiXinAccessToken( machine_code, open_id, scope, access_token, expire_time - 200 );
    }

    std::string KFWeiXin::RequestPay( const std::string& data, std::shared_ptr<const KFChannelSetting> setting )
    {
        return _invalid_string;
    }
}