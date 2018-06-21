#include "KFEnterConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFEnterConfig::KFEnterConfig()
	{

	}

	KFEnterConfig::~KFEnterConfig()
	{
		
	}


	bool KFEnterConfig::LoadConfig( const char* file )
	{
		try
		{
			//KFXml kfxml( file );
			//auto config = kfxml.RootNode();
			////////////////////////////////////////////////////////////////////
			//auto setting = config.FindNode( "Setting" );
			//while ( setting.IsValid() )
			//{
			//	auto noteid = setting.GetUInt32( "NoteId" );
			//	auto timetype = setting.GetUInt32( "TimeType" );
			//	auto timevalue = setting.GetUInt32( "TimeValue" );

			//	KFResetData data;
			//	data._parent_name = setting.GetString( "ParentName" );
			//	data._key = setting.GetUInt32( "Key" );
			//	data._data_name = setting.GetString( "DataName" );
			//	data._value = setting.GetUInt32( "Value" );

			//	AddResetData( noteid, timetype, timevalue, data );
			//	setting.NextNode();
			//}
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}