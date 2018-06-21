#include "KFGroupClientConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFGroupClientConfig::KFGroupClientConfig()
	{
		_max_member_count = 4;
		_refuse_time = 300;
		_invite_valid_time = 60;
	}

	KFGroupClientConfig::~KFGroupClientConfig()
	{
		
	}

	bool KFGroupClientConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			{
				auto xmlnode = config.FindNode( "Group" );
				_max_member_count = xmlnode.GetUInt32( "MaxMemberCount" );
			}
			//////////////////////////////////////////////////////////////////////////////////
			{
				auto xmlnode = config.FindNode( "Invite" );
				_refuse_time = xmlnode.GetUInt32( "RefuseTime" );
				_invite_valid_time = xmlnode.GetUInt32( "ValidTime" );
			}
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}