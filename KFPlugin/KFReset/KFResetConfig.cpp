#include "KFResetConfig.hpp"

namespace KFrame
{
    void KFResetConfig::ReadSetting( KFNode& xmlnode, KFResetSetting* kfsetting )
    {
        kfsetting->_time_type = xmlnode.GetUInt32( "TimeType" );
        kfsetting->_time_value = xmlnode.GetUInt32( "TimeValue" );
        kfsetting->_time_hour = xmlnode.GetUInt32( "TimeHour" );

        // 重置信息
        KFResetData data;
        data._parent_name = xmlnode.GetString( "ParentName" );
        data._key = xmlnode.GetUInt32( "Key" );
        data._data_name = xmlnode.GetString( "DataName" );
        data._operate = xmlnode.GetUInt32( "Operate" );
        data._value = xmlnode.GetUInt32( "Value" );
        kfsetting->_reset_data_list.push_back( data );
    }

}