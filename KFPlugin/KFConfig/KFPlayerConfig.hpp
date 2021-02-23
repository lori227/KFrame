#ifndef	__KF_PLAYER_CONFIG_H__
#define	__KF_PLAYER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFPlayerSetting : public KFIntSetting
	{
	public:
		// 创建类型(1=新账号,2=新角色)
		uint32 _type = 0u;

		// 获得的属性
		KFElements _reward;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFPlayerConfig : public KFConfigT<KFPlayerSetting>, public KFInstance<KFPlayerConfig>
	{
	public:
		KFPlayerConfig()
		{
			_key_name = "row";
			_file_name = "player";
		}

		~KFPlayerConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				KFGlobal::Instance()->ParseElement( setting->_reward, _file_name.c_str(), setting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFPlayerSetting> setting )
		{
			setting->_type = xml_node.ReadUInt32( "type", true );
			setting->_reward._str_parse = xml_node.ReadString( "reward", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
