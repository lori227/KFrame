#ifndef	__KF_CONDITION_CONFIG_H__
#define	__KF_CONDITION_CONFIG_H__

#include "KFConfig.h"
#include "KFConditionDefineConfig.hpp"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFConditionSetting : public KFIntSetting
	{
	public:
		// 完成条件定义
		KFConditionDefineOption _condition;

		// 条件完成数值
		uint32 _done_type = 0u;

		// 计数累计限制
		uint32 _done_value = 0u;

		// 清除计数条件定义
		KFConditionDefineOption _clean;

		// 动态限制条件
		DynamicConditionGroupPtr _dynamic_condition_limit;

		// 限制条件
		StaticConditionListPtr _static_condition_limit;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFConditionConfig : public KFConfigT<KFConditionSetting>, public KFInstance<KFConditionConfig>
	{
	public:
		KFConditionConfig()
		{
			_key_name = "id";
			_file_name = "condition";
		}

		~KFConditionConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				setting->_condition._condition_define = KFConditionDefineConfig::Instance()->FindSetting( setting->_condition._str_condition );
				if ( !setting->_condition._str_condition.empty() && setting->_condition._condition_define == nullptr )
				{
					__LOG_ERROR__( "condition=[{}] can't find define=[{}]", setting->_id, setting->_condition._str_condition );
				}

				setting->_clean._condition_define = KFConditionDefineConfig::Instance()->FindSetting( setting->_clean._str_condition );
				if ( !setting->_clean._str_condition.empty() && setting->_clean._condition_define == nullptr )
				{
					__LOG_ERROR__( "condition=[{}] can't find define=[{}]", setting->_id, setting->_clean._str_condition );
				}

			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFConditionSetting> setting )
		{
			setting->_condition._str_condition = xml_node.ReadString( "condition", true );
			setting->_done_type = xml_node.ReadUInt32( "donetype", true );
			setting->_done_value = xml_node.ReadUInt32( "donevalue", true );
			setting->_clean._str_condition = xml_node.ReadString( "clean", true );
			setting->_dynamic_condition_limit = xml_node.ReadDynamicConditionGroup( "dynamicconditionlimit", true );
			setting->_static_condition_limit = xml_node.ReadStaticConditionList( "staticconditionlimit", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
