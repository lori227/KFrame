#include "KFResetConfig.hpp"

namespace KFrame
{
    void KFResetConfig::ClearSetting()
    {
        _reset_settings.Clear();
        KFConfigT< KFResetSetting >::ClearSetting();
    }

    KFResetSetting* KFResetConfig::CreateSetting( KFNode& xmlnode )
    {
        return _settings.Create( 0 );
    }

    void KFResetConfig::ReadSetting( KFNode& xmlnode, KFResetSetting* kfsetting )
    {
        auto timeid = xmlnode.GetUInt32( "TimeId" );
        auto resetsetting = _reset_settings.Create( timeid );

        // 重置信息
        KFResetData resetdata;
        resetdata._parent_name = xmlnode.GetString( "ParentName", true );
        resetdata._key = xmlnode.GetUInt32( "Key", true );
        resetdata._data_name = xmlnode.GetString( "DataName" );
        resetdata._operate = xmlnode.GetUInt32( "Operate" );
        resetdata._value = xmlnode.GetUInt32( "Value", true );
        resetsetting->_reset_data_list.push_back( resetdata );
    }
}