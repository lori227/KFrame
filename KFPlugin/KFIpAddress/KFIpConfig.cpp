#include "KFIpConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFIpConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        _ip_address_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();

        //////////////////////////////////////////////////////////////////
        auto lognode = config.FindNode( "LogServer" );
        _log_url = lognode.GetString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto authnode = config.FindNode( "AuthServer" );
        _auth_url = authnode.GetString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto dirnode = config.FindNode( "DirServer" );
        _dir_url = dirnode.GetString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto paynode = config.FindNode( "PayServer" );
        _pay_url = authnode.GetString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto vpnnodes = config.FindNode( "Vpns" );
        if ( vpnnodes.IsValid() )
        {
            auto vpnnode = vpnnodes.FindNode( "Vpn" );
            while ( vpnnode.IsValid() )
            {
                auto zoneid = vpnnode.GetUInt32( "Zone" );
                auto ip = vpnnode.GetString( "Ip" );
                _vpn_list[ zoneid ] = ip;

                vpnnode.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }

    const std::string& KFIpConfig::FindVPNIpAddress( const std::string& appname, const std::string& apptype, uint32 zoneid )
    {
        if ( appname != __STRING__( zone ) || apptype != __STRING__( gate ) )
        {
            return _invalid_string;
        }

        auto iter = _vpn_list.find( zoneid );
        if ( iter == _vpn_list.end() )
        {
            return _invalid_string;
        }

        return iter->second;
    }
}