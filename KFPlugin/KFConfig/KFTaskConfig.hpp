#ifndef	__KF_TASK_CONFIG_H__
#define	__KF_TASK_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFTaskSetting : public KFIntSetting
	{
	public:
		// ����
		uint32 _type = 0u;

		// Ʒ��
		uint32 _quality = 0u;

		// ��ɷ�ʽ
		uint32 _complete_mode = 0u;

		bool _complete_remove = false;

		// �������
		DynamicConditionGroupPtr _complete_condition;

		// �������
		UInt32Vector _execute;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFTaskConfig : public KFConfigT<KFTaskSetting>, public KFInstance<KFTaskConfig>
	{
	public:
		KFTaskConfig()
		{
			_key_name = "id";
			_file_name = "task";
		}

		~KFTaskConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFTaskSetting> setting )
		{
			setting->_type = xml_node.ReadUInt32( "type", true );
			setting->_quality = xml_node.ReadUInt32( "quality", true );
			setting->_complete_mode = xml_node.ReadUInt32( "completemode", true );
			setting->_complete_remove = xml_node.ReadBool("completeremove", true );
			setting->_complete_condition = xml_node.ReadDynamicConditionGroup( "completecondition", true );
			setting->_execute = xml_node.ReadUInt32Vector( "execute", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
