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
	class KFGiftConfig : public KFConfigT< KFGiftSetting >, public KFInstance< KFGiftConfig >
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
			for ( auto& iter : _settings._objects )
			{
				auto kfsetting = iter.second;

				KFGlobal::Instance()->ParseElement( kfsetting->_reward, _file_name.c_str(), kfsetting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFGiftSetting* kfsetting )
		{
			kfsetting->_drop_id = xmlnode.ReadString( "dropid", true );
			kfsetting->_reward._str_parse = xmlnode.ReadString( "reward", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
