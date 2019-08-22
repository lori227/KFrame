#include "KFResetConfig.hpp"

namespace KFrame
{
    KFResetSetting* KFResetConfig::CreateSetting( KFNode& xmlnode )
    {
        auto id = _settings.Size();
        return _settings.Create( ++id );
    }
    void KFResetConfig::ReadSetting( KFNode& xmlnode, KFResetSetting* kfsetting )
    {
        kfsetting->_time_data._type = xmlnode.GetUInt32( "TimeType" );
        kfsetting->_time_data._value = xmlnode.GetUInt32( "TimeValue", true );
        kfsetting->_time_data._hour = xmlnode.GetUInt32( "TimeHour", true );

        // 重置信息
        kfsetting->_parent_name = xmlnode.GetString( "ParentName", true );
        kfsetting->_key = xmlnode.GetUInt32( "Key", true );
        kfsetting->_data_name = xmlnode.GetString( "DataName" );
        kfsetting->_operate = xmlnode.GetUInt32( "Operate" );
        kfsetting->_value = xmlnode.GetUInt32( "Value", true );
    }

}