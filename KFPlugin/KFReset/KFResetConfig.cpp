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
        kfsetting->_time_id = xmlnode.GetUInt32( "TimeId" );

        // 重置信息
        kfsetting->_parent_name = xmlnode.GetString( "ParentName", true );
        kfsetting->_key = xmlnode.GetUInt32( "Key", true );
        kfsetting->_data_name = xmlnode.GetString( "DataName" );
        kfsetting->_operate = xmlnode.GetUInt32( "Operate" );
        kfsetting->_value = xmlnode.GetUInt32( "Value", true );
    }

    void KFResetConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            kfsetting->_time_setting = KFTimeConfig::Instance()->FindSetting( kfsetting->_time_id );
            if ( kfsetting->_time_setting == nullptr )
            {
                __LOG_ERROR__( "timeid=[{}] can't find setting!", kfsetting->_time_id );
            }
        }
    }

}