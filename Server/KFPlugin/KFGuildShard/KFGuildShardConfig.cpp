#include "KFGuildShardConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFGuildShardConfig::KFGuildShardConfig()
	{
		_clean_hour = 0;
		_clean_minute = 0;
	}

	KFGuildShardConfig::~KFGuildShardConfig()
	{
		
	}
	

	bool KFGuildShardConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			//////////////////////////////////////////////////////////////////
			auto toast = config.FindNode( "Toast" );
			_clean_hour = toast.GetUInt32( "CleanHour" );
			_clean_minute = toast.GetUInt32( "CleanMinute" );

		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}