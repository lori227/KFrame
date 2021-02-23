#ifndef	__KF_SIGN_CONFIG_H__
#define	__KF_SIGN_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFSignSetting : public KFIntSetting
	{
	public:
		// Ç©µ½½±Àø
		KFElements _reward;

		// ¶îÍâ½±Àø¸ÅÂÊ
		uint32 _probability = 0u;

		// ¶îÍâµÄ½±Àø
		KFElements _extend_reward;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFSignConfig : public KFConfigT<KFSignSetting>, public KFInstance<KFSignConfig>
	{
	public:
		KFSignConfig()
		{
			_key_name = "id";
			_file_name = "sign";
		}

		~KFSignConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				KFGlobal::Instance()->ParseElement( setting->_reward, _file_name.c_str(), setting->_row );
				KFGlobal::Instance()->ParseElement( setting->_extend_reward, _file_name.c_str(), setting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFSignSetting> setting )
		{
			setting->_reward._str_parse = xml_node.ReadString( "reward", true );
			setting->_probability = xml_node.ReadUInt32( "probability", true );
			setting->_extend_reward._str_parse = xml_node.ReadString( "extendreward", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
