#include "KFConnectionConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFConnectionConfig::KFConnectionConfig()
	{
		_listen_port = 10000;
	}

	KFConnectionConfig::~KFConnectionConfig()
	{
		
	}

	const std::string& KFConnectionConfig::FindPlatformAddress( uint32 id )
	{
		auto platformid = _platform_hash.FindHashNode( id );
		auto iter = _platform_address.find( platformid );
		if ( iter == _platform_address.end() )
		{
			return _invalid_str;
		}

		return iter->second;
	}

	const KFConnection* KFConnectionConfig::FindMasterConnection( const std::string& name ) const
	{
		auto iter = _master_connection.find( name );
		if ( iter == _master_connection.end() )
		{
			return nullptr;
		}

		return &iter->second;
	}


	void KFConnectionConfig::SetMasterConnection( const std::string& name, uint32 zoneid )
	{
		auto iter = _master_connection.find( name );
		if ( iter == _master_connection.end() )
		{
			return;
		}

		auto kfconnection = &iter->second;
		kfconnection->_id += zoneid;
	}
	
	bool KFConnectionConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();
			//////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////
			auto listenport = config.FindNode( "ListenPort" );
			_listen_port = listenport.GetUInt32( "Port" );
			//////////////////////////////////////////////////////////////////
			auto masters = config.FindNode( "Masters" );
			auto master = masters.FindNode( "Master" );
			while ( master.IsValid() )
			{
				KFConnection kfconnection;
				kfconnection._name = master.GetString( "Name" );
				kfconnection._type = master.GetString( "Type" );
				kfconnection._id = master.GetUInt32( "Id" );
				kfconnection._ip = master.GetString( "Ip" );
				kfconnection._port = master.GetUInt32( "Port" );
				kfconnection._key = master.GetString( "Key" );
				_master_connection[ kfconnection._name ] = kfconnection;

				master.NextNode();
			}
			//////////////////////////////////////////////////////////////////
			auto platforms = config.FindNode( "Platforms" );
			if ( platforms.IsValid() )
			{
				auto platform = platforms.FindNode( "Platform" );
				while ( platform.IsValid() )
				{
					auto id = platform.GetUInt32( "Id" );
					auto address = platform.GetString( "Address" );
					_platform_address[ id ] = address;
				
					_platform_hash.AddHashNode( "platform", id, 100 );
					platform.NextNode();
				}
			}
			//////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////


		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}