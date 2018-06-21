#include "KFTcpClientConfig.h"

namespace KFrame
{
	bool KFTcpClientConfig::NeedToConnection( const std::string& name, const std::string& type, uint32 id ) const
	{
		for ( auto iter : _kf_connections )
		{
			auto kfconnection = &iter;
			
			if ( !kfconnection->_name.empty() )
			{
				if ( kfconnection->_name != name )
				{
					continue;
				}
			}

			if ( !kfconnection->_type.empty() )
			{
				if ( kfconnection->_type != type )
				{
					continue;
				}
			}
			
			if ( kfconnection->_id != 0 )
			{
				if ( kfconnection->_id != id )
				{
					continue;
				}
			}

			return true;
		}
		
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool KFTcpClientConfig::LoadConfig( const char* file )
	{
		_max_queue_size = 10000;

		try
		{
			KFXml kfxml( file );

			auto config = kfxml.RootNode();

			//////////////////////////////////////////////////////////////////
			auto master = config.FindNode( "Master" );
			if ( master.IsValid() )
			{
				_kf_master_connection._name = master.GetString( "Name" );
			}
			//////////////////////////////////////////////////////////////////
			auto connectionnode = config.FindNode( "Connections" );
			if ( connectionnode.IsValid() )
			{
				auto childnode = connectionnode.FindNode( "Connection" );
				while ( childnode.IsValid() )
				{
					KFConnection connection;
					connection._name = childnode.GetString( "Name" );
					connection._type = childnode.GetString( "Type" );
					connection._id = childnode.GetUInt32( "Id", true );
					_kf_connections.push_back( connection );

					childnode.NextNode();
				}
			}
			//////////////////////////////////////////////////////////////////
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}