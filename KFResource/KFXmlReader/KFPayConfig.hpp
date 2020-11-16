#ifndef	__KF_PAY_CONFIG_H__
#define	__KF_PAY_CONFIG_H__

#include "KFConfig.h"
#include "KFElementConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFPaySetting : public KFIntSetting
	{
	public:
		// Id
		uint32 _id = 0u;

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
	class KFPayConfig : public KFConfigT< KFPaySetting >, public KFInstance< KFPayConfig >
	{
	public:
		KFPayConfig()
		{
			_key_name = "id";
			_file_name = "pay";
		}

		~KFPayConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _settings._objects )
			{
				auto kfsetting = iter.second;
				KFElementConfig::Instance()->ParseElement( kfsetting->_cost_data, _file_name, kfsetting->_row );
				KFElementConfig::Instance()->ParseElement( kfsetting->_compound_data, _file_name, kfsetting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFPaySetting* kfsetting )
		{
			kfsetting->_id = xmlnode.ReadUInt32( "id" );
			kfsetting->_data_name = xmlnode.ReadString( "dataname" );
			kfsetting->_data_key = xmlnode.ReadUInt32( "datakey" );
			kfsetting->_cost_data._str_parse = xmlnode.ReadString( "costdata" );
			kfsetting->_compound_data._str_parse = xmlnode.ReadString( "compounddata" );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
