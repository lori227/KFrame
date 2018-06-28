#include "KFMySQLConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	const KFMySQLSetting* KFMySQLConfig::FindSetting( uint32 id )
	{
		return _mysql_setting.Find( id );
	}

	const KFMySQLSetting* KFMySQLConfig::FindSetting( const std::string& field, uint32 logicid )
	{
		auto redisid = FindLogicMySQLId( field, logicid );
		return FindSetting( redisid );
	}

	uint32 KFMySQLConfig::FindLogicMySQLId( const std::string& filed, uint32 logicid )
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

	bool KFMySQLConfig::LoadConfig( const char* file )
	{
		_mysql_setting.Clear();
		_logic_mysql_map.clear();

		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			//////////////////////////////////////////////////////////////////
			auto databasenode = config.FindNode( "MySQL" );
			if ( databasenode.IsValid() )
			{
				auto childnode = databasenode.FindNode( "Connection" );
				while ( childnode.IsValid() )
				{
					auto kfsetting = __KF_CREATE__( KFMySQLSetting );

					kfsetting->_id = childnode.GetUInt32( "Id" );
					kfsetting->_ip = childnode.GetString( "IP" );
					kfsetting->_port = childnode.GetUInt32( "Port" );
					kfsetting->_password = childnode.GetString( "Password" );
					kfsetting->_database = childnode.GetString( "Database" );
					kfsetting->_user = childnode.GetString( "User" );
					//kfsetting._name = childnode.GetString( "Name" );
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