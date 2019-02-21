#include "KFIpAddressConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFIpAddressConfig::LoadConfig()
    {
        _ip_address_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();

        auto authnode = config.FindNode( "AuthServer" );
        _auth_url = authnode.GetString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto lognode = config.FindNode( "LogServer" );
        _log_url = lognode.GetString( "Url" );
        //////////////////////////////////////////////////////////////////

        auto kfglobal = KFGlobal::Instance();

        auto tcpnode = config.FindNode( "TcpServer" );
        if ( tcpnode.IsValid() )
        {
            auto servernode = tcpnode.FindNode( "Server" );
            while ( servernode.IsValid() )
            {
                auto strappid = servernode.GetString( "AppId" );

                KFAppId appid( strappid );
                auto zoneid = appid.GetZoneId();
                if ( zoneid == _invalid_int || zoneid == kfglobal->_app_id->GetZoneId() )
                {
                    KFIpAddress kfaddress;

                    appid.SetChannelId( kfglobal->_app_id->GetChannelId() );
                    kfaddress._app_id = appid.GetId();
                    kfaddress._str_app_id = appid.ToString();

                    // 集群 或者 相同小区
                    kfaddress._app_name = servernode.GetString( "AppName" );
                    kfaddress._app_type = servernode.GetString( "AppType" );
                    kfaddress._ip = servernode.GetString( "Ip" );
                    kfaddress._port_type = servernode.GetUInt32( "Type" );
                    kfaddress._port = servernode.GetUInt32( "Port" );
                    _ip_address_list.push_back( kfaddress );
                }

                servernode.NextNode();
            }
        }

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
}