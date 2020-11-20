#ifndef	__KF_FILTER_CONFIG_H__
#define	__KF_FILTER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFFilterSetting : public KFIntSetting
	{
	public:
		// ÆÁ±Î×Ö
		std::string _word;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFFilterConfig : public KFConfigT< KFFilterSetting >, public KFInstance< KFFilterConfig >
	{
	public:
		KFFilterConfig()
		{
			_key_name = "row";
			_file_name = "filter";
		}

		~KFFilterConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFFilterSetting* kfsetting )
		{
			kfsetting->_word = xmlnode.ReadString( "word", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
