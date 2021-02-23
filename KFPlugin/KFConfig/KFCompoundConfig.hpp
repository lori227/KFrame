#ifndef	__KF_COMPOUND_CONFIG_H__
#define	__KF_COMPOUND_CONFIG_H__

#include "KFConfig.h"

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
	class KFCompoundConfig : public KFConfigT<KFCompoundSetting>, public KFInstance<KFCompoundConfig>
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
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				KFGlobal::Instance()->ParseElement( setting->_cost_data, _file_name.c_str(), setting->_row );
				KFGlobal::Instance()->ParseElement( setting->_compound_data, _file_name.c_str(), setting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFCompoundSetting> setting )
		{
			setting->_data_name = xml_node.ReadString( "dataname", true );
			setting->_data_key = xml_node.ReadUInt32( "datakey", true );
			setting->_cost_data._str_parse = xml_node.ReadString( "costdata", true );
			setting->_compound_data._str_parse = xml_node.ReadString( "compounddata", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
