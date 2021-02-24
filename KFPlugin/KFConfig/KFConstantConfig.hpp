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
		// …Ë÷√ ˝÷µ
		std::string _value;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFConstantConfig : public KFConfigT<KFConstantSetting>, public KFInstance<KFConstantConfig>
	{
	public:
		KFConstantConfig()
		{
			_key_name = "name";
			_file_name = "constant";
		}

		~KFConstantConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFConstantSetting> setting )
		{
			setting->_value = xml_node.ReadString( "value", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
