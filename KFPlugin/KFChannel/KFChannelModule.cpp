#include "KFChannelModule.hpp"
#include "KFInternal.hpp"
#include "KFWeiXin.hpp"
#include "KFSteam.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFChannelModule::~KFChannelModule()
    {
        _kf_channel_list.Clear();
        _kf_pay_function.Clear();
        _kf_login_function.Clear();
    }

    void KFChannelModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////////////
        RegisterChannel< KFInternal >( KFMsg::Internal );
        RegisterChannel< KFWeiXin >( KFMsg::WeiXin );
        RegisterChannel< KFWeiXin >( KFMsg::Steam );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFChannelModule::IsOpen( std::shared_ptr<const KFChannelSetting> setting ) const
    {
#ifdef __KF_DEBUG__
        return setting->_debug;
#else
        return setting->_release;
#endif
    }

    bool KFChannelModule::IsSupport( std::shared_ptr<const KFChannelSetting> setting, uint32 channel ) const
    {
        if ( setting->_support.empty() )
        {
            return false;
        }

        return setting->_support.find( channel ) != setting->_support.end();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////


    std::string KFChannelModule::AuthLogin( KFJson& request )
    {
        auto channel = __JSON_GET_UINT32__( request, __STRING__( channel ) );
        if ( channel != KFGlobal::Instance()->_channel )
        {
            // 渠道不同, 判断是否支持
            auto setting = KFChannelConfig::Instance()->FindSetting( KFGlobal::Instance()->_channel );
            if ( !IsSupport( setting, channel ) )
            {
                return _kf_http_server->SendCode( KFMsg::ChannelNotSupport );
            }
        }

        // 渠道是否开启
        auto setting = KFChannelConfig::Instance()->FindSetting( channel );
        if ( setting == nullptr || !IsOpen( setting ) )
        {
            return _kf_http_server->SendCode( KFMsg::ChannelNotOpen );
        }

        // 查找回调函数
        auto login_function_data = _kf_login_function.Find( channel );
        if ( login_function_data == nullptr )
        {
            return _kf_http_server->SendCode( KFMsg::ChannelError );
        }

        // 执行回调
        return login_function_data->CallEx<std::string>( request, setting );
    }

    std::string KFChannelModule::AuthPay( uint32 channel, const std::string& data )
    {
        if ( channel != KFGlobal::Instance()->_channel )
        {
            // 渠道不同, 判断是否支持
            auto setting = KFChannelConfig::Instance()->FindSetting( KFGlobal::Instance()->_channel );
            if ( !IsSupport( setting, channel ) )
            {
                __LOG_ERROR__( "channel=[{}] not support", channel );
                return _invalid_string;
            }
        }

        // 渠道是否开启
        auto setting = KFChannelConfig::Instance()->FindSetting( channel );
        if ( setting == nullptr || !IsOpen( setting ) )
        {
            __LOG_ERROR__( "channel=[{}] no setting", channel );
            return _invalid_string;
        }

        auto pay_function_data = _kf_pay_function.Find( channel );
        if ( pay_function_data == nullptr )
        {
            __LOG_ERROR__( "channel=[{}] no function", channel );
            return _invalid_string;
        }

        return pay_function_data->CallEx<std::string>( data, setting );
    }
}