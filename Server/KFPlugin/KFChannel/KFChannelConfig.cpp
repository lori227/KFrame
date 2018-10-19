#include "KFChannelConfig.h"

namespace KFrame
{
    bool KFChannelSetting::IsOpen() const
    {
#ifdef __KF_DEBUG__
        return _debug_open;
#else
        return _release_open;
#endif
    }

    bool KFChannelConfig::IsChannelOpen( uint32 channel )
    {
        auto kfchannelsetting = _kf_channel.Find( channel );
        if ( kfchannelsetting == nullptr )
        {
            return false;
        }

        return kfchannelsetting->IsOpen();
    }

    const KFChannelSetting* KFChannelConfig::FindChannelSetting( uint32 channel )
    {
        return _kf_channel.Find( channel );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFChannelConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto channels = config.FindNode( "Channels" );
        auto channel = channels.FindNode( "Channel" );
        while ( channel.IsValid() )
        {
            auto channelid = channel.GetUInt32( "Id" );
            auto kfchannelsetting = _kf_channel.Create( channelid );

            kfchannelsetting->_channel_id = channelid;
            kfchannelsetting->_login_url = channel.GetString( "LoginUrl" );
            kfchannelsetting->_pay_url = channel.GetString( "PayUrl" );
            kfchannelsetting->_app_id = channel.GetString( "AppId" );
            kfchannelsetting->_app_key = channel.GetString( "AppKey" );
            kfchannelsetting->_release_open = channel.GetBoolen( "Release" );
            kfchannelsetting->_debug_open = channel.GetBoolen( "Debug" );

            channel.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}