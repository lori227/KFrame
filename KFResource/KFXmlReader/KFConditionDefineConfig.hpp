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
		virtual void ReadSetting( KFXmlNode& xmlnode, KFConditionDefineSetting* kfsetting )
		{
			kfsetting->_parent_name = xmlnode.ReadString( "parentname", true );
			kfsetting->_data_name = xmlnode.ReadString( "dataname", true );
			kfsetting->_init_calc_type = xmlnode.ReadUInt32( "initcalctype", true );
			kfsetting->_save_uuid = xmlnode.ReadBoolen( "saveuuid", true );
		
			ConditionTrigger conditiontrigger;
			conditiontrigger._call_type = xmlnode.ReadUInt32( "calltype" );
			conditiontrigger._trigger_type = xmlnode.ReadUInt32( "triggertype" );
			conditiontrigger._trigger_check = xmlnode.ReadUInt32( "triggercheck" );
			conditiontrigger._trigger_use = xmlnode.ReadUInt32( "triggeruse" );
			conditiontrigger._trigger_value = xmlnode.ReadUInt32( "triggervalue" );
			conditiontrigger._use_operate = xmlnode.ReadUInt32( "useoperate" );
			conditiontrigger._use_type = xmlnode.ReadUInt32( "usetype" );
			conditiontrigger._use_value = xmlnode.ReadUInt32( "usevalue" );
			kfsetting->_condition_trigger.push_back( conditiontrigger );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
