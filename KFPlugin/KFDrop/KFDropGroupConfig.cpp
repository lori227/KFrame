#include "KFDropGroupConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropGroupConfig::ReadSetting( KFXmlNode& xmlnode, KFDropSetting* kfsetting )
    {
        kfsetting->_is_drop_count = xmlnode.ReadBoolen( "dropcount", true );
        kfsetting->_is_exclude = xmlnode.ReadBoolen( "exclude", true );
        kfsetting->_condition_type = xmlnode.ReadUInt32( "conditiontype", true );
        kfsetting->_rand_type = xmlnode.ReadUInt32( "randtype", true );

        std::shared_ptr<KFDropGroupWeight> drop_weight = nullptr;
        auto weight = xmlnode.ReadUInt32( "weight", true );

        if ( weight == KFRandEnum::TenThousand )
        {
            drop_weight = __MAKE_SHARED__( KFDropGroupWeight );
            kfsetting->_necessary_list.Add( drop_weight );
            drop_weight->_value = kfsetting->_necessary_list._objects.size();
        }
        else
        {
            auto id = kfsetting->_rand_list.Size();
            drop_weight = kfsetting->_rand_list.Create( ++id, weight );
        }

        drop_weight->_is_clear_var = xmlnode.ReadBoolen( "reset", true );
        drop_weight->_drop_data_id = xmlnode.ReadUInt32( "dropdataid", true );

        // 条件
        drop_weight->_conditions = xmlnode.ReadStaticConditions( "condition", true );
    }

    void KFDropGroupConfig::LoadAllComplete()
    {
        for ( auto iter : _settings._objects )
        {
            auto setting = iter.second;
            for ( auto& drop_group_weight : setting->_necessary_list._objects )
            {
                InitDropDataSetting( setting, drop_group_weight );
            }

            for ( auto drop_group_weight : setting->_rand_list._weight_data )
            {
                InitDropDataSetting( setting, drop_group_weight );
            }
        }
    }

    void KFDropGroupConfig::InitDropDataSetting( KFDropSetting* setting, std::shared_ptr<KFDropGroupWeight>& drop_group_weight )
    {
        drop_group_weight->_drop_data_setting = KFDropDataConfig::Instance()->FindSetting( drop_group_weight->_drop_data_id );
        if ( drop_group_weight->_drop_data_setting == nullptr )
        {
            if ( drop_group_weight->_drop_data_id != 0u )
            {
                __LOG_ERROR__( "dropid=[{}] dropdata=[{}] can't find setting", setting->_id, drop_group_weight->_drop_data_id );
            }
        }

        // 判断掉落条件
        for ( auto condition : drop_group_weight->_conditions->_condition_list )
        {
            InitDropConditonSetting( setting, condition->_left );
            InitDropConditonSetting( setting, condition->_right );
        }
    }

    void KFDropGroupConfig::InitDropConditonSetting( KFDropSetting* kfsetting, KFStaticConditionExpression* kfexpression )
    {
        if ( kfexpression == nullptr )
        {
            return;
        }

        for ( auto kfdata : kfexpression->_data_list )
        {
            if ( !kfdata->IsVariable() )
            {
                continue;
            }

            auto kfvariable = ( KFStaticConditionVariable* )kfdata;
            if ( kfvariable->_data_name == __STRING__( drop ) )
            {
                kfvariable->_parent_name = __STRING__( drop );
                kfvariable->_data_name = __STRING__( value );
                kfvariable->_data_id = kfsetting->_id;
            }
        }
    }
}