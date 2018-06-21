#include "KFClusterServerConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFClusterServerConfig::KFClusterServerConfig()
	{

	}

	KFClusterServerConfig::~KFClusterServerConfig()
	{
		
	}
	
	bool KFClusterServerConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();
			//////////////////////////////////////////////////////////////////
			auto cluster = config.FindNode( "ClusterServer" );

			_cluster_key = cluster.GetString( "ClusterKey" );

			//////////////////////////////////////////////////////////////////
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}