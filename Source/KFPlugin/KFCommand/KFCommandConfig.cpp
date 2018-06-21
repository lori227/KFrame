#include "KFCommandConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFCommandConfig::KFCommandConfig()
	{
	}

	KFCommandConfig::~KFCommandConfig()
	{
		
	}
	
	bool KFCommandConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			//////////////////////////////////////////////////////////////////
			//auto command = config.FindNode( "Command" );
			//////////////////////////////////////////////////////////////////
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}