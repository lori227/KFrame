#include "KFChannelModule.hpp"
#include "KFInternal.hpp"
#include "KFWeiXin.hpp"
#include "KFSteam.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFChannelModule::~KFChannelModule()
    {
        for ( auto iter : _kf_channel_list )
        {
            __DELETE_OBJECT__( iter.second );
        }
        _kf_channel_list.clear();
    }


    void KFChannelModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////////////
        RegisterChannel( KFMsg::Internal, __NEW_OBJECT__( KFInternal ) );
        RegisterChannel( KFMsg::WeiXin, __NEW_OBJECT__( KFWeiXin ) );
        RegisterChannel( KFMsg::Steam, __NEW_OBJECT__( KFSteam ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFChannelModule::IsOpen( const KFChannelSetting* kfsetting ) const
    {
#ifdef __KF_DEBUG__
        return kfsetting->_debug;
#else
        return kfsetting->_release;
#endif
    }

    bool KFChannelModule::IsSupport( const KFChannelSetting* kfsetting, uint32 channel ) const
    {
        if ( kfsetting->_support.empty() )
        {
            return false;
        }

        return kfsetting->_support.find( channel ) != kfsetting->_support.end();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////


    std::string KFChannelModule::AuthLogin( KFJson& request )
    {
        auto channel = __JSON_GET_UINT32__( request, __STRING__( channel ) );
        if ( channel != KFGlobal::Instance()->_channel )
        {
            // 渠道不同, 判断是否支持
            auto kfsetting = KFChannelConfig::Instance()->FindSetting( KFGlobal::Instance()->_channel );
            if ( !IsSupport( kfsetting, channel ) )
            {
                return _kf_http_server->SendCode( KFMsg::ChannelNotSupport );
            }
        }

        // 渠道是否开启
        auto kfsetting = KFChannelConfig::Instance()->FindSetting( channel );
        if ( kfsetting == nullptr || !IsOpen( kfsetting ) )
        {
            return _kf_http_server->SendCode( KFMsg::ChannelNotOpen );
        }

        // 查找回调函数
        auto kffunction = _kf_login_function.Find( channel );
        if ( kffunction == nullptr )
        {
            return _kf_http_server->SendCode( KFMsg::ChannelError );
        }

        // 执行回调
        return kffunction->_function( request, kfsetting );
    }

    std::string KFChannelModule::AuthPay( uint32 channel, const std::string& data )
    {
        if ( channel != KFGlobal::Instance()->_channel )
        {
            // 渠道不同, 判断是否支持
            auto kfsetting = KFChannelConfig::Instance()->FindSetting( KFGlobal::Instance()->_channel );
            if ( !IsSupport( kfsetting, channel ) )
            {
                __LOG_ERROR__( "channel=[{}] not support", channel );
                return _invalid_string;
            }
        }

        // 渠道是否开启
        auto kfsetting = KFChannelConfig::Instance()->FindSetting( channel );
        if ( kfsetting == nullptr || !IsOpen( kfsetting ) )
        {
            __LOG_ERROR__( "channel=[{}] no setting", channel );
            return _invalid_string;
        }

        auto kffunction = _kf_pay_function.Find( channel );
        if ( kffunction == nullptr )
        {
            __LOG_ERROR__( "channel=[{}] no function", channel );
            return _invalid_string;
        }

        return kffunction->_function( data, kfsetting );
    }
}