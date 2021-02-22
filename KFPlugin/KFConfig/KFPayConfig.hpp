#ifndef	__KF_PAY_CONFIG_H__
#define	__KF_PAY_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFPaySetting : public KFStrSetting
	{
	public:
		// 价格
		uint32 _price = 0u;

		// 充值奖励
		KFElements _reward;

		// 纪录充值标记id
		uint32 _note_id = 0u;

		// 首冲奖励
		KFElements _first_reward;

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
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				KFGlobal::Instance()->ParseElement( setting->_reward, _file_name.c_str(), setting->_row );
				KFGlobal::Instance()->ParseElement( setting->_first_reward, _file_name.c_str(), setting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFPaySetting> setting )
		{
			setting->_price = xml_node.ReadUInt32( "price", true );
			setting->_reward._str_parse = xml_node.ReadString( "reward", true );
			setting->_note_id = xml_node.ReadUInt32( "noteid", true );
			setting->_first_reward._str_parse = xml_node.ReadString( "firstreward", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
