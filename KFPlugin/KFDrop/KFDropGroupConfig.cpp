#include "KFDropGroupConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropGroupConfig::ReadSetting( KFXmlNode& xml_node, KFDropSetting* setting )
    {
        setting->_is_drop_count = xml_node.ReadBool( "dropcount", true );
        setting->_is_exclude = xml_node.ReadBool( "exclude", true );
        setting->_condition_type = xml_node.ReadUInt32( "conditiontype", true );
        setting->_rand_type = xml_node.ReadUInt32( "randtype", true );

        std::shared_ptr<KFDropGroupWeight> drop_weight = nullptr;
        auto weight = xml_node.ReadUInt32( "weight", true );

        if ( weight == KFRandEnum::TenThousand )
        {
            drop_weight = __MAKE_SHARED__( KFDropGroupWeight );
            setting->_necessary_list.Add( drop_weight );
            drop_weight->_value = setting->_necessary_list._objects.size();
        }
        else
        {
            auto id = setting->_rand_list.Size();
            drop_weight = setting->_rand_list.Create( ++id, weight );
        }

        drop_weight->_is_clear_var = xml_node.ReadBool( "reset", true );
        drop_weight->_drop_data_id = xml_node.ReadUInt32( "dropdataid", true );

        // 条件
        drop_weight->_conditions = xml_node.ReadStaticConditions( "condition", true );
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

    void KFDropGroupConfig::InitDropConditonSetting( KFDropSetting* setting, KFStaticConditionExpression* kfexpression )
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
                kfvariable->_data_id = setting->_id;
            }
        }
    }
}