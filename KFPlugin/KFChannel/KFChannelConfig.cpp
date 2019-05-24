#include "KFChannelConfig.hpp"

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
        if ( _open_channel_list.empty() )
        {
            return true;
        }

        auto iter = _open_channel_list.find( channel );
        return iter != _open_channel_list.end();
    }

    const KFChannelSetting* KFChannelConfig::FindChannelSetting( uint32 channel )
    {
        return _kf_channel.Find( channel );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFChannelConfig::LoadConfig( const std::string& file )
    {
        _open_channel_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();

        auto channels = config.FindNode( "Channels" );
        auto channel = channels.FindNode( "Channel" );
        while ( channel.IsValid() )
        {
            auto service = channel.GetUInt32( "Service", true, _invalid_int );
            if ( KFGlobal::Instance()->CheckChannelService( _invalid_int, service ) )
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
            }

            channel.NextNode();
        }

        auto opens = config.FindNode( "Opens" );
        auto opennode = opens.FindNode( "Open" );
        while ( opennode.IsValid() )
        {
            auto channelid = opennode.GetUInt32( "ChannelId" );
            auto service = opennode.GetUInt32( "Service" );
            if ( KFGlobal::Instance()->CheckChannelService( channelid, service ) )
            {
                auto strlist = opennode.GetString( "List" );
                while ( !strlist.empty() )
                {
                    _open_channel_list.insert( KFUtility::SplitValue< uint32 >( strlist, "," ) );
                }

                break;
            }

            opennode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}