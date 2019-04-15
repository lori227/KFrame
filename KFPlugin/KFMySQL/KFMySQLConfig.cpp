#include "KFMySQLConfig.hpp"

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
        auto _mysql_id = 0u;
        _mysql_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto mysqlnode = config.FindNode( "MySQL" );

        auto modulenode = mysqlnode.FindNode( "Module" );
        while ( modulenode.IsValid() )
        {
            auto modulename = modulenode.GetString( "Name" );
            auto logicid = modulenode.GetUInt32( "LogicId", true, _invalid_int );
            auto kfsetting = _mysql_setting.Create( ModuleKey( modulename, logicid ) );

            kfsetting->_id = ++_mysql_id;
            kfsetting->_ip = modulenode.GetString( "IP" );
            kfsetting->_port = modulenode.GetUInt32( "Port" );
            kfsetting->_database = modulenode.GetString( "Database" );
            kfsetting->_user = modulenode.GetString( "User" );
            kfsetting->_password = modulenode.GetString( "Password" );

            modulenode.NextNode();
        }

        return true;
    }
}