#include "KFResetConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFResetConfig::KFResetConfig()
    {

    }

    KFResetConfig::~KFResetConfig()
    {

    }

    void KFResetConfig::AddResetData( uint32 noteid, uint32 resettype, uint32 resettime, uint32 timehour, const KFResetData& resetdata )
    {
        auto kfsetting = _reset_setting.Create( noteid );
        if ( kfsetting->_note_id == _invalid_int )
        {
            kfsetting->_note_id = noteid;
            kfsetting->_time_type = resettype;
            kfsetting->_time_value = resettime;
            kfsetting->_time_hour = timehour;
        }

        kfsetting->_reset_data_list.push_back( resetdata );
    }

    bool KFResetConfig::LoadConfig()
    {
        _reset_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto setting = config.FindNode( "Setting" );
        while ( setting.IsValid() )
        {
            auto noteid = setting.GetUInt32( "NoteId" );
            auto timetype = setting.GetUInt32( "TimeType" );
            auto timevalue = setting.GetUInt32( "TimeValue" );
            auto timehour = setting.GetUInt32( "TimeHour" );

            KFResetData data;
            data._parent_name = setting.GetString( "ParentName" );
            data._key = setting.GetUInt32( "Key" );
            data._data_name = setting.GetString( "DataName" );
            data._operate = setting.GetUInt32( "Operate" );
            data._value = setting.GetUInt32( "Value" );

            AddResetData( noteid, timetype, timevalue, timehour, data );
            setting.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}