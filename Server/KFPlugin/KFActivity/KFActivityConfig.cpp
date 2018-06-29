#include "KFActivityConfig.h"

namespace KFrame
{

    void KFActivitySetting::AddActivityData( KFActivityData* data )
    {
        _datas[ data->_id ] = data;
        if ( _max_value < data->_done_value )
        {
            _max_value = data->_done_value;
        }
    }

    const KFActivityData* KFActivitySetting::FindActivityData( uint32 id ) const
    {
        auto iter = _datas.find( id );
        if ( iter == _datas.end() )
        {
            return nullptr;
        }

        return iter->second;
    }

    bool KFActivitySetting::CheckInTime( uint64 nowtime, uint64 createtime ) const
    {
        switch ( _time_type )
        {
        case KFActivityEnum::DynamicTime:
            return !KFDate::CheckPassTime( nowtime, createtime, _duration );
            break;
        default:
            break;
        }

        return KFDate::CheckInTime( _start_time, _end_time, nowtime );
    }

    bool KFActivitySetting::CheckResetActivity( uint64 nowtime, uint64 createtime, uint64 eventtime ) const
    {
        // 不在活动时间范围内了
        if ( !CheckInTime( eventtime, createtime ) )
        {
            return true;
        }

        // 是否每日清除
        switch ( _reset_type )
        {
        case KFActivityEnum::ResetDaily:
        {
            if ( KFDate::CheckPassDay( eventtime, nowtime ) )
            {
                return true;
            }
            break;
        }
        default:
            break;
        }

        return false;
    }

    bool KFActivitySetting::CheckCanUpdate( uint32 key, uint32 level, uint32 operate ) const
    {
        if ( level < _level )
        {
            return false;
        }

        if ( _key != 0 && _key != key )
        {
            return false;
        }

        if ( _trigger_type == 0 )
        {
            return true;
        }

        if ( _trigger_type == operate )
        {
            return true;
        }

        return false;
    }

    uint32 KFActivitySetting::CheckTriggerValue( uint32 operatevalue, uint32 nowvalue ) const
    {
        switch ( _use_type )
        {
        case KFActivityEnum::UseFinal:
            operatevalue = nowvalue;
            break;
        default:
            break;
        }

        if ( operatevalue < _trigger_value )
        {
            return 0;
        }

        return operatevalue;
    }

    uint32 KFActivitySetting::GetUseValue( uint32 operatevalue ) const
    {
        if ( _use_value == 0 )
        {
            return operatevalue;
        }

        return _use_value;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    const KFActivitySetting* KFActivityConfig::FindActivitySetting( uint32 type ) const
    {
        return _activity_setting.Find( type );
    }

    const KFActivityData* KFActivityConfig::FindActivityData( uint32 type, uint32 id ) const
    {
        auto psetting = FindActivitySetting( type );
        if ( psetting == nullptr )
        {
            return nullptr;
        }

        return psetting->FindActivityData( id );
    }

    void KFActivityConfig::AddActivitySetting( KFActivitySetting* setting, KFActivityData* eventdata )
    {
        setting->AddActivityData( eventdata );

        AddActivityType( setting );
    }

    void KFActivityConfig::AddActivityType( KFActivitySetting* setting )
    {
        auto key = ActivityTypeKey( setting->_parent_name, setting->_data_name );
        auto iter = _activity_types.find( key );
        if ( iter != _activity_types.end() )
        {
            return;
        }
        KFActivityType kfactivitytype;
        kfactivitytype.AddKFAchieveType( setting );
        _activity_types[ key ] = kfactivitytype;
        //iter->second.AddKFAchieveType( setting );
    }

    const KFActivityType* KFActivityConfig::FindActivityType( const std::string& parentname, const std::string& dataname ) const
    {
        auto key = ActivityTypeKey( parentname, dataname );
        auto iter = _activity_types.find( key );
        if ( iter == _activity_types.end() )
        {
            return nullptr;
        }

        return &iter->second;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFActivityConfig::KFActivityConfig()
    {

    }

    KFActivityConfig::~KFActivityConfig()
    {
        _activity_setting.Clear();
    }

    bool KFActivityConfig::LoadConfig( const char* file )
    {
        _activity_setting.Clear();

        try
        {
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}