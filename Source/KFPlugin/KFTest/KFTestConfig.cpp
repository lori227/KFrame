#include "KFTestConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFTestConfig::KFTestConfig()
	{
		_is_client = false;
		_client_count = 0;
		_message_count = 0;
	}

	KFTestConfig::~KFTestConfig()
	{
		
	}
	
	bool KFTestConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			auto setting = config.FindNode( "Setting" );
			_is_client = setting.GetBoolen( "IsClient" );
			_client_count = setting.GetUInt32( "ClientCount" );
			_message_count = setting.GetUInt32( "MessageCount" );
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}