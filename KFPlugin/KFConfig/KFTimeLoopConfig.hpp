#ifndef	__KF_TIMELOOP_CONFIG_H__
#define	__KF_TIMELOOP_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFTimeLoopSetting : public KFIntSetting
	{
	public:
		// Ë¢ÐÂÊ±¼ä
		KFTimeData _time_data;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFTimeLoopConfig : public KFConfigT< KFTimeLoopSetting >, public KFInstance< KFTimeLoopConfig >
	{
	public:
		KFTimeLoopConfig()
		{
			_key_name = "id";
			_file_name = "timeloop";
		}

		~KFTimeLoopConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFTimeLoopSetting> setting )
		{
		
			KFTimeData timedata;
			timedata._flag = xml_node.ReadUInt32( "flag", true );
			timedata._month = xml_node.ReadUInt32( "month", true );
			timedata._day = xml_node.ReadUInt32( "day", true );
			timedata._day_of_week = xml_node.ReadUInt32( "dayofweek", true );
			timedata._hour = xml_node.ReadUInt32( "hour", true );
			timedata._minute = xml_node.ReadUInt32( "minute", true );
			setting->_time_data = timedata;
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
