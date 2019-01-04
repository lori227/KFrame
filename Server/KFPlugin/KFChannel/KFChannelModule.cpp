#include "KFChannelModule.h"
#include "KFChannelConfig.h"
#include "KFInternal.h"
#include "KFWeiXin.h"
#include "KFSteam.h"
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
        RegisterChannel( KFMsg::Internal, new KFInternal() );
        RegisterChannel( KFMsg::WeiXin, new KFWeiXin( ) );
        RegisterChannel( KFMsg::Steam, new KFSteam() );
    }

    void KFChannelModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_channel_config );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFChannelModule::AuthChannelLogin( const std::string& data )
    {
        __JSON_PARSE_STRING__( request, data );

        auto channel = __JSON_GET_UINT32__( request, __KF_STRING__( channel ) );
        if ( !_kf_channel_config->IsChannelOpen( channel ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::ChannelNotSupport );
        }

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