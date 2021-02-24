#ifndef	__KF_GIFT_CONFIG_H__
#define	__KF_GIFT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFGiftSetting : public KFIntSetting
	{
	public:
		// µôÂäid
		std::string _drop_id;

		// ½±Àø
		KFElements _reward;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFGiftConfig : public KFConfigT<KFGiftSetting>, public KFInstance<KFGiftConfig>
	{
	public:
		KFGiftConfig()
		{
			_key_name = "id";
			_file_name = "gift";
		}

		~KFGiftConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				KFGlobal::Instance()->ParseElement( setting->_reward, _file_name.c_str(), setting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFGiftSetting> setting )
		{
			setting->_drop_id = xml_node.ReadString( "dropid", true );
			setting->_reward._str_parse = xml_node.ReadString( "reward", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
