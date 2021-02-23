#ifndef	__KF_TEAM_CONFIG_H__
#define	__KF_TEAM_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFTeamSetting : public KFIntSetting
	{
	public:
		// 队伍最大数量
		uint32 _max_count = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFTeamConfig : public KFConfigT<KFTeamSetting>, public KFInstance<KFTeamConfig>
	{
	public:
		KFTeamConfig()
		{
			_key_name = "id";
			_file_name = "team";
		}

		~KFTeamConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFTeamSetting> setting )
		{
			setting->_max_count = xml_node.ReadUInt32( "maxcount", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
