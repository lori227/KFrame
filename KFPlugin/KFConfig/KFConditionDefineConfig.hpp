#ifndef	__KF_CONDITIONDEFINE_CONFIG_H__
#define	__KF_CONDITIONDEFINE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class ConditionTrigger
	{
	public:
		// ���»ص���ʽ
		uint32 _call_type = 0u;

		// ��������
		uint32 _trigger_type = 0u;

		// �������ʽ
		uint32 _trigger_check = 0u;

		// ����ֵʹ������
		uint32 _trigger_use = 0u;

		// ����ֵ
		uint32 _trigger_value = 0u;

		// ��������
		uint32 _use_operate = 0u;

		// ʹ�õ�����
		uint32 _use_type = 0u;

		// ���õ�ʹ����ֵ
		uint32 _use_value = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFConditionDefineSetting : public KFStrSetting
	{
	public:
		// ��������
		std::string _parent_name;

		// ��������
		std::string _data_name;

		// ��ʼֵ
		uint32 _init_calc_type = 0u;

		// �Ƿ񱣴�uuid
		bool _save_uuid = false;

		// ������������
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
			kfsetting->_parent_name = xmlnode.ReadString( "parent_name", true );
			kfsetting->_data_name = xmlnode.ReadString( "data_name", true );
			kfsetting->_init_calc_type = xmlnode.ReadUInt32( "initcalctype", true );
			kfsetting->_save_uuid = xmlnode.ReadBoolen( "saveuuid", true );
		
			ConditionTrigger conditiontrigger;
			conditiontrigger._call_type = xmlnode.ReadUInt32( "calltype", true );
			conditiontrigger._trigger_type = xmlnode.ReadUInt32( "triggertype", true );
			conditiontrigger._trigger_check = xmlnode.ReadUInt32( "triggercheck", true );
			conditiontrigger._trigger_use = xmlnode.ReadUInt32( "triggeruse", true );
			conditiontrigger._trigger_value = xmlnode.ReadUInt32( "triggervalue", true );
			conditiontrigger._use_operate = xmlnode.ReadUInt32( "useoperate", true );
			conditiontrigger._use_type = xmlnode.ReadUInt32( "usetype", true );
			conditiontrigger._use_value = xmlnode.ReadUInt32( "usevalue", true );
			kfsetting->_condition_trigger.push_back( conditiontrigger );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
