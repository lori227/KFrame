#include "KFBusConfig.hpp"

namespace KFrame
{

    bool KFBusConfig::IsValidConnection( const std::string& connectname, const std::string& connecttype, const std::string& connectid )
    {
        auto kfglobal = KFGlobal::Instance();

        for ( auto& kfconnection : _bus_connection )
        {
            if ( kfconnection._app_name != _globbing_string && kfconnection._app_name != kfglobal->_app_name )
            {
                continue;
            }

            if ( kfconnection._app_type != _globbing_string && kfconnection._app_type != kfglobal->_app_type )
            {
                continue;
            }

            if ( kfconnection._app_id != _globbing_string && kfconnection._app_id != kfglobal->_app_id->ToString() )
            {
                continue;
            }

            // 判断连接目标信息
            if ( kfconnection._connect_name != _globbing_string && kfconnection._connect_name != connectname )
            {
                continue;
            }

            if ( kfconnection._connect_type != _globbing_string && kfconnection._connect_type != connecttype )
            {
                continue;
            }

            if ( kfconnection._connect_id != _globbing_string && kfconnection._connect_id != connectid )
            {
                continue;
            }

            // 连接间隔
            if ( kfconnection._interval != _invalid_int && kfconnection._multi != _invalid_int )
            {
                KFAppId kfappid( connectid );
                auto connectworkid = kfappid.GetWorkId();
                auto workid = ( kfglobal->_app_id->GetWorkId() + kfconnection._multi - 1 ) / kfconnection._multi;
                if ( ( __MAX__( workid, connectworkid ) ) - ( __MIN__( workid, connectworkid ) ) > kfconnection._interval )
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    const KFConnection* KFBusConfig::FindMasterConnection( const std::string& appname, const std::string& apptype, const std::string& appid )
    {
        for ( auto& kfconnection : _bus_connection )
        {
            if ( kfconnection._app_name != _globbing_string && kfconnection._app_name != appname )
            {
                continue;
            }

            if ( kfconnection._app_type != _globbing_string && kfconnection._app_type != apptype )
            {
                continue;
            }

            if ( kfconnection._app_id != _globbing_string && kfconnection._app_id != appid )
            {
                continue;
            }

            if ( kfconnection._connect_name != _globbing_string && kfconnection._app_name != appname )
            {
                continue;
            }

            if ( kfconnection._connect_type != _globbing_string && kfconnection._connect_type != __STRING__( master ) )
            {
                continue;
            }

            return &kfconnection;
        }

        return nullptr;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFBusConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        _bus_connection.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();
        auto busnode = config.FindNode( "Bus" );
        if ( busnode.IsValid() )
        {
            auto connectionnode = busnode.FindNode( "Connection" );
            while ( connectionnode.IsValid() )
            {
                KFConnection kfconnection;

                kfconnection._app_name = connectionnode.GetString( "AppName" );
                kfconnection._app_type = connectionnode.GetString( "AppType" );
                kfconnection._app_id = connectionnode.GetString( "AppId" );

                kfconnection._connect_name = connectionnode.GetString( "ConnectName" );
                kfconnection._connect_type = connectionnode.GetString( "ConnectType" );
                kfconnection._connect_id = connectionnode.GetString( "ConnectId" );

                kfconnection._interval = connectionnode.GetUInt32( "Interval", true, _invalid_int );
                kfconnection._multi = connectionnode.GetUInt32( "Multi", true, _invalid_int );

                _bus_connection.push_back( kfconnection );

                connectionnode.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }
}