#include "KFAchieveConfig.hpp"

namespace KFrame
{
    void KFAchieveConfig::ClearSetting()
    {
        _achieve_types.clear();
        KFConfigT< KFAchieveSetting >::ClearSetting();
    }

    void KFAchieveConfig::ReadSetting( KFNode& xmlnode, KFAchieveSetting* kfsetting )
    {
        kfsetting->_parent_name = xmlnode.GetString( "ParentName" );
        kfsetting->_data_name = xmlnode.GetString( "DataName" );
        kfsetting->_data_key = xmlnode.GetUInt32( "DataKey", true );

        kfsetting->_trigger_type = xmlnode.GetUInt32( "TriggerType" );
        kfsetting->_trigger_check = xmlnode.GetUInt32( "TriggerCheck", true );
        kfsetting->_trigger_value = xmlnode.GetUInt32( "TriggerValue", true );

        kfsetting->_use_type = xmlnode.GetUInt32( "UseType" );
        kfsetting->_use_value = xmlnode.GetUInt32( "UseValue", true );
        kfsetting->_use_operate = xmlnode.GetUInt32( "UseOperate" );

        kfsetting->_done_value = xmlnode.GetUInt32( "DoneValue" );
        kfsetting->_done_type = xmlnode.GetUInt32( "DoneType" );

        auto strlimit = xmlnode.GetString( "Limits", true );
        if ( !strlimit.empty() )
        {
            kfsetting->_limits.Parse( strlimit, __FUNC_LINE__ );
        }

        auto strrewards = xmlnode.GetString( "Rewards" );
        if ( !strrewards.empty() )
        {
            kfsetting->_rewards.Parse( strrewards, __FUNC_LINE__ );
        }

        AddAchieveType( kfsetting );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void KFAchieveTypes::AddAchieveType( KFAchieveSetting* setting )
    {
        auto iter = _achieve_type.find( setting->_data_name );
        if ( iter == _achieve_type.end() )
        {
            iter = _achieve_type.insert( std::make_pair( setting->_data_name, KFAchieveType() ) ).first;
        }

        iter->second.AddKFAchieveType( setting );
    }

    const KFAchieveType* KFAchieveTypes::FindAchieveType( const std::string& dataname ) const
    {
        auto iter = _achieve_type.find( dataname );
        if ( iter == _achieve_type.end() )
        {
            return nullptr;
        }

        return &iter->second;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void KFAchieveConfig::AddAchieveType( KFAchieveSetting* kfsetting )
    {
        if ( kfsetting->_parent_name.empty() || kfsetting->_data_name.empty() )
        {
            return;
        }

        auto iter = _achieve_types.find( kfsetting->_parent_name );
        if ( iter == _achieve_types.end() )
        {
            iter = _achieve_types.insert( std::make_pair( kfsetting->_parent_name, KFAchieveTypes() ) ).first;
        }

        iter->second.AddAchieveType( kfsetting );
    }

    const KFAchieveType* KFAchieveConfig::FindTypeAchieve( const std::string& parentname, const std::string& dataname ) const
    {
        auto iter = _achieve_types.find( parentname );
        if ( iter == _achieve_types.end() )
        {
            return nullptr;
        }

        return iter->second.FindAchieveType( dataname );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFAchieveSetting::CheckCanUpdate( uint64 key, uint64 operate ) const
    {
        if ( _data_key != 0u && _data_key != key )
        {
            return false;
        }

        if ( _trigger_type == 0u || _trigger_type == operate )
        {
            return true;
        }

        return false;
    }

    uint64 KFAchieveSetting::CheckTriggerValue( uint64 operatevalue, uint64 nowvalue ) const
    {
        // 触发值
        if ( _use_type == KFEnum::UseFinal )
        {
            operatevalue = nowvalue;
        }

        if ( _trigger_check != 0 )
        {
            auto ok = KFUtility::CheckOperate<uint64>( operatevalue, _trigger_check, _trigger_value );
            if ( !ok )
            {
                return 0u;
            }
        }

        // 最终使用值
        if ( _use_value != 0u )
        {
            operatevalue = _use_value;
        }

        return operatevalue;
    }
}