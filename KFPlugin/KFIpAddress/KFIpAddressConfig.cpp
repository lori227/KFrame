#include "KFIpAddressConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KFIpAddressConfig::LoadConfig( const std::string& file )
    {
        _ip_address_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();

        auto authnode = config.FindNode( "AuthServer" );
        _auth_url = authnode.GetString( "Url" );
        //////////////////////////////////////////////////////////////////
        auto lognode = config.FindNode( "LogServer" );
        _log_url = lognode.GetString( "Url" );
        //////////////////////////////////////////////////////////////////

        auto kfglobal = KFGlobal::Instance();

        auto tcpnode = config.FindNode( "TcpServer" );
        auto servernode = tcpnode.FindNode( "Server" );
        while ( servernode.IsValid() )
        {
            auto appname = servernode.GetString( "AppName" );
            auto apptype = servernode.GetString( "AppType" );

            auto appnode = servernode.FindNode( "App" );
            while ( appnode.IsValid() )
            {
                auto strappid = appnode.GetString( "Id" );

                // 集群 或者 相同小区
                KFAppId appid( strappid );
                auto zoneid = appid.GetZoneId();
                if ( zoneid == _invalid_int || zoneid == kfglobal->_app_id->GetZoneId() )
                {
                    if ( appname == "zone" )
                    {
                        appid.SetZoneId( kfglobal->_app_id->GetZoneId() );
                    }

                    KFIpAddress kfaddress;
                    kfaddress._name = appname;
                    kfaddress._type = apptype;
                    kfaddress._id = appid.GetId();
                    kfaddress._str_id = appid.ToString();
                    kfaddress._ip = appnode.GetString( "Ip" );
                    kfaddress._port = appnode.GetUInt32( "Port" );
                    kfaddress._port_type = appnode.GetUInt32( "Type" );
                    _ip_address_list[ kfaddress._id ] = kfaddress;
                }

                appnode.NextNode();
            }

            servernode.NextNode();
        }
    }

    const KFIpAddress* KFIpAddressConfig::FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid )
    {
        IpAddressList iplist;
        FindIpAddress( appname, apptype, appid, iplist );

        auto count = static_cast< uint32 >( iplist.size() );
        if ( count == 0u )
        {
            return nullptr;
        }

        auto kfglobal = KFGlobal::Instance();
        auto workerid = kfglobal->_app_id->GetWorkId();

        auto index = workerid % count;
        return iplist.at( index );
    }

    void KFIpAddressConfig::FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid, IpAddressList& outlist )
    {
        for ( auto& iter : _ip_address_list )
        {
            auto& kfaddress = iter.second;
            if ( appname != _globbing_str && appname != kfaddress._name  )
            {
                continue;
            }

            if ( apptype != _globbing_str && apptype != kfaddress._type )
            {
                continue;
            }

            if ( appid != _globbing_str && appid != kfaddress._str_id )
            {
                continue;
            }

            outlist.push_back( &kfaddress );
        }
    }
}