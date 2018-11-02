#include "KFMySQLConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFMySQLSetting* KFMySQLConfig::FindSetting( const std::string& module, uint32 logicid ) const
    {
        auto key = ModuleKey( module, logicid );
        return _mysql_setting.Find( key );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFMySQLConfig::LoadConfig()
    {
        auto _redis_id = 0u;
        _mysql_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto mysqlnode = config.FindNode( "MySQL" );

        auto modulenode = mysqlnode.FindNode( "Connection" );
        while ( modulenode.IsValid() )
        {
            auto modulename = modulenode.GetString( "Module" );
            auto logicid = modulenode.GetUInt32( "LogicId", true, _invalid_int );
            auto kfsetting = _mysql_setting.Create( ModuleKey( modulename, logicid ) );

            kfsetting->_id = ++_redis_id;
            kfsetting->_ip = modulenode.GetString( "IP" );
            kfsetting->_port = modulenode.GetUInt32( "Port" );
            kfsetting->_database = modulenode.GetString( "Database" );
            kfsetting->_user = modulenode.GetString( "User" );
            kfsetting->_password = modulenode.GetString( "Password" );

            auto channelnode = modulenode.FindNode( "Channel" );
            while ( channelnode.IsValid() )
            {
                auto channelid = channelnode.GetUInt32( "ChannelId" );
                auto service = channelnode.GetUInt32( "Service" );
                if ( KFGlobal::Instance()->CheckChannelService( channelid, service ) )
                {
                    kfsetting->_ip = channelnode.GetString( "IP" );
                    kfsetting->_port = channelnode.GetUInt32( "Port" );
                    kfsetting->_database = channelnode.GetString( "Database" );
                    kfsetting->_user = channelnode.GetString( "User" );
                    kfsetting->_password = channelnode.GetString( "Password" );
                    break;
                }

                channelnode.NextNode();
            }

            modulenode.NextNode();
        }

        return true;
    }
}