#include "KFChannelModule.h"
#include "KFChannelConfig.h"
#include "KFJson.h"
#include "KFInternal.h"
#include "KFWeiXin.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFChannelModule::KFChannelModule()
    {
    }

    KFChannelModule::~KFChannelModule()
    {
        for ( auto iter : _kf_channel_list )
        {
            delete iter.second;
        }
        _kf_channel_list.clear();
    }

    void KFChannelModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_channel_config, true );
    }

    void KFChannelModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////////////
        RegisterChannel( new KFInternal( KFMsg::Internal ) );
        RegisterChannel( new KFWeiXin( KFMsg::WeiXin ) );

    }

    void KFChannelModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFChannelModule::AuthChannelLogin( const std::string& data )
    {
        KFJson request( data );

        auto channel = request.GetUInt32( __KF_STRING__( channel ) );

        // 渠道是否开启
        auto kfsetting = _kf_channel_config->FindChannelSetting( channel );
        if ( kfsetting == nullptr || !kfsetting->IsOpen() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::ChannelNotOpen );
        }

        // 查找回调函数
        auto kffunction = _kf_login_function.Find( channel );
        if ( kffunction == nullptr )
        {
            return _kf_http_server->SendResponseCode( KFMsg::ChannelError );
        }

        // 执行回调
        return kffunction->_function( request, kfsetting );
    }
}