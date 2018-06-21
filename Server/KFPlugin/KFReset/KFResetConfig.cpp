#include "KFResetConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFResetConfig::KFResetConfig()
	{

	}

	KFResetConfig::~KFResetConfig()
	{
		
	}

	void KFResetConfig::AddResetData( uint64 noteid, uint32 resettype, uint32 resettime, const KFResetData& resetdata )
	{
		auto kfsetting = _reset_setting.Create( noteid );
		if ( kfsetting->_note_id == _invalid_int )
		{
			kfsetting->_note_id = noteid;
			kfsetting->_reset_time_type = resettype;
			kfsetting->_reset_time_value = resettime;
		}

		kfsetting->_reset_data_list.push_back( resetdata );
	}
	
	bool KFResetConfig::LoadConfig( const char* file )
	{
		_reset_setting.Clear();

		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();
			//////////////////////////////////////////////////////////////////
			auto setting = config.FindNode( "Setting" );
			while ( setting.IsValid() )
			{
				auto noteid = setting.GetUInt32( "NoteId" );
				auto timetype = setting.GetUInt32( "TimeType" );
				auto timevalue = setting.GetUInt32( "TimeValue" );

				KFResetData data;
				data._parent_name = setting.GetString( "ParentName" );
				data._key = setting.GetUInt32( "Key" );
				data._data_name = setting.GetString( "DataName" );
				data._value = setting.GetUInt32( "Value" );

				AddResetData( noteid, timetype, timevalue, data );
				setting.NextNode();
			}
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}