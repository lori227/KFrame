#include "KFMySQLConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFMySQLSetting* KFMySQLConfig::FindSetting( uint32 id ) const
    {
        return _mysql_setting.Find( id );
    }

    const KFMySQLSetting* KFMySQLConfig::FindSetting( const std::string& field, uint32 logicid ) const
    {
        auto redisid = FindLogicMySQLId( field, logicid );
        return FindSetting( redisid );
    }

    uint32 KFMySQLConfig::FindLogicMySQLId( const std::string& filed, uint32 logicid ) const
    {
        auto key = LogicKey( filed, logicid );
        auto iter = _logic_mysql_map.find( key );
        if ( iter == _logic_mysql_map.end() )
        {
            return _invalid_int;
        }

        return iter->second;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFMySQLConfig::LoadConfig()
    {
        _mysql_setting.Clear();
        _logic_mysql_map.clear();

        auto kfglobal = KFGlobal::Instance();
        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto databasenode = config.FindNode( "MySQL" );
            if ( databasenode.IsValid() )
            {
                auto childnode = databasenode.FindNode( "Connection" );
                while ( childnode.IsValid() )
                {
                    auto kfsetting = __KF_CREATE__( KFMySQLSetting );

                    kfsetting->_id = childnode.GetUInt32( "MySqlId" );
                    kfsetting->_ip = childnode.GetString( "IP" );
                    kfsetting->_port = childnode.GetUInt32( "Port" );
                    kfsetting->_password = childnode.GetString( "Password" );
                    kfsetting->_database = childnode.GetString( "Database" );
                    kfsetting->_user = childnode.GetString( "User" );

                    auto channelnode = childnode.FindNode( "Channel" );
                    while ( channelnode.IsValid() )
                    {
                        auto channelid = channelnode.GetUInt32( "ChannelId" );
                        if ( channelid == kfglobal->_app_channel )
                        {
                            kfsetting->_ip = channelnode.GetString( "IP" );
                            kfsetting->_port = channelnode.GetUInt32( "Port" );
                            kfsetting->_password = channelnode.GetString( "Password" );
                            kfsetting->_database = channelnode.GetString( "Database" );
                            kfsetting->_user = channelnode.GetString( "User" );
                            break;
                        }

                        channelnode.NextNode();
                    }

                    _mysql_setting.Insert( kfsetting->_id, kfsetting );
                    childnode.NextNode();
                }
            }

            auto redis = config.FindNode( "LogicMySQL" );
            auto logic = redis.FindNode( "Logic" );
            while ( logic.IsValid() )
            {
                auto dataname = logic.GetString( "Filed" );
                auto logicid = logic.GetUInt32( "LogicId" );
                auto redisid = logic.GetUInt32( "Id" );

                auto key = LogicKey( dataname, logicid );
                _logic_mysql_map[ key ] = redisid;

                logic.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}