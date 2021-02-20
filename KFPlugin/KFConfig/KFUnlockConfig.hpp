#ifndef	__KF_UNLOCK_CONFIG_H__
#define	__KF_UNLOCK_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFUnlockSetting : public KFIntSetting
	{
	public:
		// ������
		std::string _data_name;

		// ����id
		uint32 _data_key = 0u;

		// ����������
		std::string _child_name;

		// ����ֵ
		uint32 _data_value = 0u;

		// ��������
		DynamicConditionGroupPtr _unlock_condition;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFUnlockConfig : public KFConfigT< KFUnlockSetting >, public KFInstance< KFUnlockConfig >
	{
	public:
		KFUnlockConfig()
		{
			_key_name = "id";
			_file_name = "unlock";
		}

		~KFUnlockConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFUnlockSetting* kfsetting )
		{
			kfsetting->_data_name = xmlnode.ReadString( "data_name", true );
			kfsetting->_data_key = xmlnode.ReadUInt32( "datakey", true );
			kfsetting->_child_name = xmlnode.ReadString( "child_name", true );
			kfsetting->_data_value = xmlnode.ReadUInt32( "datavalue", true );
			kfsetting->_unlock_condition = xmlnode.ReadDynamicConditionGroup( "unlockcondition", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
