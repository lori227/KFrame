#include "KFAchieveConfig.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    void KFDataAchieveTypes::AddKFAchieveType( KFAchieveSetting* setting )
    {
        auto iter = _achievetype.find( setting->_data_name );
        if ( iter == _achievetype.end() )
        {
            iter = _achievetype.insert( std::make_pair( setting->_data_name, KFAchieveType() ) ).first;
        }

        iter->second.AddKFAchieveType( setting );
    }

    const KFAchieveType* KFDataAchieveTypes::FindKFAchieveType( const std::string& dataname ) const
    {
        auto iter = _achievetype.find( dataname );
        if ( iter == _achievetype.end() )
        {
            return nullptr;
        }

        return &iter->second;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    KFAchieveConfig::KFAchieveConfig()
    {

    }

    KFAchieveConfig::~KFAchieveConfig()
    {
        _achieve_setting.Clear();
        _object_types.clear();
        _battle_achieve_setting.Clear();

    }

    const KFAchieveSetting* KFAchieveConfig::FindAchieveSetting( uint32 id ) const
    {
        return _achieve_setting.Find( id );
    }

    void KFAchieveConfig::AddKFAchieveSetting( KFAchieveSetting* kfsetting )
    {
        AddKFTypeAchieve( kfsetting );
        _achieve_setting.Insert( kfsetting->_id, kfsetting );
    }

    void KFAchieveConfig::AddKFTypeAchieve( KFAchieveSetting* setting )
    {
        auto iter = _object_types.find( setting->_parent_name );
        if ( iter == _object_types.end() )
        {
            iter = _object_types.insert( std::make_pair( setting->_parent_name, KFDataAchieveTypes() ) ).first;
        }

        iter->second.AddKFAchieveType( setting );
    }

    const KFAchieveType* KFAchieveConfig::FindTypeAchieve( const std::string& parentname, const std::string& dataname ) const
    {
        auto iter = _object_types.find( parentname );
        if ( iter == _object_types.end() )
        {
            return nullptr;
        }

        return iter->second.FindKFAchieveType( dataname );
    }

    bool KFAchieveSetting::CheckCanUpdate( uint32 key, uint32 level, uint32 operate ) const
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

    uint32 KFAchieveSetting::CheckTriggerValue( uint32 operatevalue, uint32 nowvalue ) const
    {
        switch ( _use_type )
        {
        case KFAchieveEnum::UseFinal:
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

    uint32 KFAchieveSetting::GetUseValue( uint32 operatevalue ) const
    {
        if ( _use_value == 0 )
        {
            return operatevalue;
        }

        return _use_value;
    }

    bool KFAchieveConfig::LoadConfig()
    {
        _achieve_setting.Clear();
        _object_types.clear();

        try
        {
            //////////////////////////////////////////////////////////////////
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto xmlnode = config.FindNode( "Setting" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFAchieveSetting );

                kfsetting->_id = xmlnode.GetUInt32( "Id" );
                kfsetting->_level = xmlnode.GetUInt32( "Level" );
                kfsetting->_trigger_type = xmlnode.GetUInt32( "TriggerType" );
                kfsetting->_trigger_value = xmlnode.GetUInt32( "TriggerValue" );
                kfsetting->_use_type = xmlnode.GetUInt32( "UseType" );
                kfsetting->_use_value = xmlnode.GetUInt32( "UseValue" );
                kfsetting->_key = xmlnode.GetUInt32( "Key" );
                kfsetting->_operate = xmlnode.GetUInt32( "Operate" );
                kfsetting->_done_value = xmlnode.GetUInt32( "DoneValue" );
                kfsetting->_done_type = xmlnode.GetUInt32( "DoneType" );
                kfsetting->_parent_name = xmlnode.GetString( "ParentName" );
                kfsetting->_data_name = xmlnode.GetString( "DataName" );

                auto rewards = xmlnode.GetString( "Rewards" );
                kfsetting->_rewards.ParseFromString( rewards, __FUNC_LINE__ );

                if ( KFAchieveEnum::lobby == xmlnode.GetUInt32( "HandleType" ) )
                {
                    AddKFAchieveSetting( kfsetting );
                }

                else
                {
                    _battle_achieve_setting.Insert( kfsetting->_id, kfsetting );
                }

                xmlnode.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}