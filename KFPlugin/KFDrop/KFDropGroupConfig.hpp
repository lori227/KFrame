#ifndef __KF_DROP_GROUP_CONFIG_H__
#define __KF_DROP_GROUP_CONFIG_H__

#include "KFConfig.h"
#include "KFDropDataConfig.hpp"

namespace KFrame
{
    class KFDropGroupWeight : public KFWeight
    {
    public:
        // 是否要清除变量
        bool _is_clear_var = false;

        // 掉落数据id
        uint32 _drop_data_id = 0u;

        // 条件
        StaticConditionsPtr _conditions;

        // 掉落数据配置
        std::shared_ptr<const KFDropDataSetting> _drop_data_setting = nullptr;
    };

    class KFDropSetting : public KFIntSetting
    {
    public:
        enum MyEnum
        {
            MutexCondition = 1,		// 互斥条件
            OverlayCondition = 2,	// 叠加条件
        };

    public:
        // 是否纪录掉落次数
        bool _is_drop_count = false;

        // 是否排除相同的
        bool _is_exclude = false;

        // 随机类型
        uint32 _rand_type = 0u;

        // 条件类型
        uint32 _condition_type = 0u;

        // 必掉的数据
        KFVector<KFDropGroupWeight> _necessary_list;

        // 掉落数据
        KFWeightList<KFDropGroupWeight> _rand_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    class KFDropGroupConfig : public KFConfigT<KFDropSetting>, public KFInstance<KFDropGroupConfig>
    {
    public:
        KFDropGroupConfig()
        {
            _key_name = "id";
            _file_name = "dropgroup";
        }

        // 全部读取
        virtual void LoadAllComplete()
        {
            for ( auto iter : _setting_list._objects )
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

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFDropSetting> setting )
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

        // 初始化掉落
        void InitDropDataSetting( std::shared_ptr<KFDropSetting> setting, std::shared_ptr<KFDropGroupWeight>& drop_group_weight )
        {
            drop_group_weight->_drop_data_setting = KFDropDataConfig::Instance()->FindSetting( drop_group_weight->_drop_data_id );
            if ( drop_group_weight->_drop_data_setting == nullptr )
            {
                if ( drop_group_weight->_drop_data_id != 0u )
                {
                    __LOG_ERROR__( "drop_id=[{}] dropdata=[{}] can't find setting", setting->_id, drop_group_weight->_drop_data_id );
                }
            }

            // 判断掉落条件
            for ( auto condition : drop_group_weight->_conditions->_condition_list )
            {
                InitDropConditionSetting( setting, condition->_left );
                InitDropConditionSetting( setting, condition->_right );
            }
        }

        // 初始化掉落条件
        void InitDropConditionSetting( std::shared_ptr<KFDropSetting> setting, std::shared_ptr<KFStaticConditionExpression> static_expression )
        {
            if ( static_expression == nullptr )
            {
                return;
            }

            for ( auto condition_data : static_expression->_data_list )
            {
                if ( !condition_data->IsVariable() )
                {
                    continue;
                }

                auto condition_variable = std::static_pointer_cast<KFStaticConditionVariable>( condition_data );
                if ( condition_variable->_data_name == __STRING__( drop ) )
                {
                    condition_variable->_parent_name = __STRING__( drop );
                    condition_variable->_data_name = __STRING__( value );
                    condition_variable->_data_id = setting->_id;
                }
            }
        }
    };
}

#endif