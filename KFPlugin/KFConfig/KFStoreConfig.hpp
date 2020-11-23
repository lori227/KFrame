#ifndef	__KF_STORE_CONFIG_H__
#define	__KF_STORE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFStoreSetting : public KFIntSetting
	{
	public:
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFStoreConfig : public KFConfigT< KFStoreSetting >, public KFInstance< KFStoreConfig >
	{
	public:
		KFStoreConfig()
		{
			_key_name = "id";
			_file_name = "store";
		}

		~KFStoreConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFStoreSetting* kfsetting )
		{
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
