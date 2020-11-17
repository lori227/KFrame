#ifndef	__KF_COMPOUND_CONFIG_H__
#define	__KF_COMPOUND_CONFIG_H__

#include "KFConfig.h"
#include "KFElementConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFCompoundSetting : public KFIntSetting
	{
	public:
		// 属性名字
		std::string _data_name;

		// 属性id
		uint32 _data_key = 0u;

		// 消耗物品
		KFElements _cost_data;

		// 合成的物品
		KFElements _compound_data;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFCompoundConfig : public KFConfigT< KFCompoundSetting >, public KFInstance< KFCompoundConfig >
	{
	public:
		KFCompoundConfig()
		{
			_key_name = "id";
			_file_name = "compound";
		}

		~KFCompoundConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _settings._objects )
			{
				auto kfsetting = iter.second;

				KFElementConfig::Instance()->ParseElement( kfsetting->_cost_data, _file_name.c_str(), kfsetting->_row );
				KFElementConfig::Instance()->ParseElement( kfsetting->_compound_data, _file_name.c_str(), kfsetting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFCompoundSetting* kfsetting )
		{
			kfsetting->_data_name = xmlnode.ReadString( "dataname", true );
			kfsetting->_data_key = xmlnode.ReadUInt32( "datakey", true );
			kfsetting->_cost_data._str_parse = xmlnode.ReadString( "costdata", true );
			kfsetting->_compound_data._str_parse = xmlnode.ReadString( "compounddata", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
