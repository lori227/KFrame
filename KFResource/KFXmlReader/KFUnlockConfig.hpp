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
		// 数据名
		std::string _data_name;

		// 数据id
		uint32 _data_key = 0u;

		// 子数据名称
		std::string _child_name;

		// 数据值
		uint32 _data_value = 0u;

		// 解锁条件
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
			kfsetting->_data_name = xmlnode.ReadString( "dataname", true );
			kfsetting->_data_key = xmlnode.ReadUInt32( "datakey", true );
			kfsetting->_child_name = xmlnode.ReadString( "childname", true );
			kfsetting->_data_value = xmlnode.ReadUInt32( "datavalue", true );
			kfsetting->_unlock_condition = xmlnode.ReadDynamicConditionGroup( "unlockcondition", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
