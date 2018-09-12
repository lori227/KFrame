#include "KFIpAddressConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFIpAddressConfig::KFIpAddressConfig()
    {
    }

    KFIpAddressConfig::~KFIpAddressConfig()
    {

    }

    bool KFIpAddressConfig::LoadConfig()
    {
        _ip_address_list.clear();
        _auth_address.clear();
        auto kfglobal = KFGlobal::Instance();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto tcpnode = config.FindNode( "TcpServer" );
        if ( tcpnode.IsValid() )
        {
            auto servernode = tcpnode.FindNode( "Server" );
            while ( servernode.IsValid() )
            {
                KFIpAddress kfaddress;
                kfaddress._app_name = servernode.GetString( "AppName" );
                kfaddress._app_type = servernode.GetString( "AppType" );
                kfaddress._str_app_id = servernode.GetString( "AppId" );
                kfaddress._ip = servernode.GetString( "Ip" );
                kfaddress._port_type = servernode.GetUInt32( "Type" );
                kfaddress._port = servernode.GetUInt32( "Port" );

                auto channelnode = servernode.FindNode( "Channel" );
                while ( channelnode.IsValid() )
                {
                    auto channelid = channelnode.GetUInt32( "ChannelId" );
                    if ( channelid == kfglobal->_app_channel )
                    {
                        kfaddress._ip = channelnode.GetString( "Ip" );
                        break;
                    }

                    channelnode.NextNode();
                }

                _ip_address_list.push_back( kfaddress );

                servernode.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////
        auto httpnode = config.FindNode( "HttpServer" );
        auto channelnode = httpnode.FindNode( "Channel" );
        while ( channelnode.IsValid() )
        {
            auto channelid = channelnode.GetUInt32( "ChannelId" );
            if ( channelid == kfglobal->_app_channel )
            {
                auto ip = channelnode.GetString( "Ip" );
                auto port = channelnode.GetString( "Port" );
                if ( port == _invalid_str )
                {
                    _auth_address = __FORMAT__( "http://{}/", ip );
                }
                else
                {
                    _auth_address = __FORMAT__( "http://{}:{}/", ip, port );
                }
                break;
            }

            channelnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    const KFIpAddress* KFIpAddressConfig::FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid )
    {
        for ( auto& kfaddress : _ip_address_list )
        {
            if ( kfaddress._app_name == appname &&
                    kfaddress._app_type == apptype &&
                    ( appid == _globbing_str || kfaddress._str_app_id == appid ) )
            {
                return &kfaddress;
            }
        }

        return nullptr;
    }

    void KFIpAddressConfig::FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid, IpAddressList& outlist )
    {
        for ( auto& kfaddress : _ip_address_list )
        {
            if ( appname != _globbing_str && appname != kfaddress._app_name  )
            {
                continue;
            }

            if ( apptype != _globbing_str && apptype != kfaddress._app_type )
            {
                continue;
            }

            if ( appid != _globbing_str && appid != kfaddress._str_app_id )
            {
                continue;
            }

            outlist.insert( &kfaddress );
        }
    }

    void KFIpAddressConfig::SetZoneIpAddress( const std::string& ip )
    {
        auto kfaddress = FindIpAddress( __KF_STRING__( zone ), __KF_STRING__( master ), _globbing_str );
        if ( kfaddress == nullptr )
        {
            return;
        }

        auto* kfzoneaddress = const_cast< KFIpAddress* >( kfaddress );
        kfzoneaddress->_ip = ip;
    }

    void KFIpAddressConfig::SetMasterAppId( uint32 appchannel, uint32 zoneid )
    {
        for ( auto& kfaddress : _ip_address_list )
        {
            KFAppID kfappid( kfaddress._str_app_id );

            kfappid._union._app_data._channel_id = appchannel;
            if ( kfaddress._app_name == __KF_STRING__( zone ) )
            {
                kfappid._union._app_data._zone_id = zoneid;
            }

            kfaddress._app_id = kfappid._union._app_id;
            kfaddress._str_app_id = kfappid.ToString();
        }
    }

}