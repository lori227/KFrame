#ifndef	__KF_CONSTANT_CONFIG_H__
#define	__KF_CONSTANT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFConstantSetting : public KFStrSetting
	{
	public:
		// �ֶ���(���ܸ�����ֶ�)
		std::string _name;

		// ������ֵ
		std::string _value;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFConstantConfig : public KFConfigT< KFConstantSetting >, public KFInstance< KFConstantConfig >
	{
	public:
		KFConstantConfig()
		{
			_key_name = "name";
			_file_name = "constant";
		}

		~KFConstantConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFConstantSetting* kfsetting )
		{
			kfsetting->_name = xmlnode.ReadString( "name", true );
			kfsetting->_value = xmlnode.ReadString( "value", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
