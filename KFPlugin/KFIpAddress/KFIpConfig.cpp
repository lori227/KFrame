#include "KFIpConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFIpConfig::LoadConfig( const std::string& filepath, uint32 cleartype )
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();
        //////////////////////////////////////////////////////////////////
        auto dnsnode = config.FindNode( "DNSServer" );
        _dns_get_ip_url = dnsnode.ReadString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto lognode = config.FindNode( "LogServer" );
        _log_url = lognode.ReadString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto authnode = config.FindNode( "AuthServer" );
        _auth_url = authnode.ReadString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto dirnode = config.FindNode( "DirServer" );
        _dir_url = dirnode.ReadString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto paynode = config.FindNode( "PayServer" );
        _pay_url = authnode.ReadString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto vpnnodes = config.FindNode( "Vpns" );
        if ( vpnnodes.IsValid() )
        {
            auto vpnnode = vpnnodes.FindNode( "Vpn" );
            while ( vpnnode.IsValid() )
            {
                auto zoneid = vpnnode.ReadUInt32( "Zone" );
                auto ip = vpnnode.ReadString( "Ip" );
                auto port = vpnnode.ReadUInt32( "Port", true );
                _vpn_list[ zoneid ] = std::make_tuple( ip, port );

                vpnnode.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }

    std::tuple<std::string, uint32> KFIpConfig::FindVPNIpAddress( const std::string& appname, const std::string& apptype, uint32 zoneid )
    {
        if ( appname != __STRING__( zone ) || apptype != __STRING__( gate ) )
        {
            return std::make_tuple( _invalid_string, 0u );
        }

        auto iter = _vpn_list.find( zoneid );
        if ( iter == _vpn_list.end() )
        {
            return std::make_tuple( _invalid_string, 0u );
        }

        return iter->second;
    }
}