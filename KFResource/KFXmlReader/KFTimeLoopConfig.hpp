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
		virtual void ReadSetting( KFXmlNode& xmlnode, KFTimeLoopSetting* kfsetting )
		{
		
			KFTimeData timedata;
			timedata._flag = xmlnode.ReadUInt32( "flag" );
			timedata._month = xmlnode.ReadUInt32( "month" );
			timedata._day = xmlnode.ReadUInt32( "day" );
			timedata._day_of_week = xmlnode.ReadUInt32( "dayofweek" );
			timedata._hour = xmlnode.ReadUInt32( "hour" );
			timedata._minute = xmlnode.ReadUInt32( "minute" );
			kfsetting->_time_data = timedata;
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
