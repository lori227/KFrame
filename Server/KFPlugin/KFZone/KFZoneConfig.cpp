#include "KFZoneConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFZoneConfig::KFZoneConfig()
	{

	}

	KFZoneConfig::~KFZoneConfig()
	{
		
	}

	bool KFZoneConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();
			//////////////////////////////////////////////////////////////////
			auto xmlnode = config.FindNode( "Zone" );

			_kf_zone._id = xmlnode.GetUInt32( "Id" );
			_kf_zone._type = xmlnode.GetUInt32( "Type" );
			_kf_zone._name = xmlnode.GetString( "Name" );
			_kf_zone._logic_id = xmlnode.GetUInt32( "LogicId", true, _kf_zone._id );
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}