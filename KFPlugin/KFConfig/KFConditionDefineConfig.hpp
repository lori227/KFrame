#ifndef	__KF_CONDITIONDEFINE_CONFIG_H__
#define	__KF_CONDITIONDEFINE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class ConditionTrigger
	{
	public:
		// 更新回调方式
		uint32 _call_type = 0u;

		// 触发条件
		uint32 _trigger_type = 0u;

		// 触发表达式
		uint32 _trigger_check = 0u;

		// 触发值使用类型
		uint32 _trigger_use = 0u;

		// 触发值
		uint32 _trigger_value = 0u;

		// 操作类型
		uint32 _use_operate = 0u;

		// 使用的类型
		uint32 _use_type = 0u;

		// 配置的使用数值
		uint32 _use_value = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFConditionDefineSetting : public KFStrSetting
	{
	public:
		// 父属性名
		std::string _parent_name;

		// 子属性名
		std::string _data_name;

		// 初始值
		uint32 _init_calc_type = 0u;

		// 是否保存uuid
		bool _save_uuid = false;

		// 条件触发数据
		std::vector<ConditionTrigger> _condition_trigger;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFConditionDefineConfig : public KFConfigT< KFConditionDefineSetting >, public KFInstance< KFConditionDefineConfig >
	{
	public:
		KFConditionDefineConfig()
		{
			_key_name = "id";
			_file_name = "conditiondefine";
		}

		~KFConditionDefineConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFConditionDefineSetting> setting )
		{
			setting->_parent_name = xml_node.ReadString( "parentname", true );
			setting->_data_name = xml_node.ReadString( "dataname", true );
			setting->_init_calc_type = xml_node.ReadUInt32( "initcalctype", true );
			setting->_save_uuid = xml_node.ReadBool( "saveuuid", true );
		
			ConditionTrigger conditiontrigger;
			conditiontrigger._call_type = xml_node.ReadUInt32( "calltype", true );
			conditiontrigger._trigger_type = xml_node.ReadUInt32( "triggertype", true );
			conditiontrigger._trigger_check = xml_node.ReadUInt32( "triggercheck", true );
			conditiontrigger._trigger_use = xml_node.ReadUInt32( "triggeruse", true );
			conditiontrigger._trigger_value = xml_node.ReadUInt32( "triggervalue", true );
			conditiontrigger._use_operate = xml_node.ReadUInt32( "useoperate", true );
			conditiontrigger._use_type = xml_node.ReadUInt32( "usetype", true );
			conditiontrigger._use_value = xml_node.ReadUInt32( "usevalue", true );
			setting->_condition_trigger.push_back( conditiontrigger );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
