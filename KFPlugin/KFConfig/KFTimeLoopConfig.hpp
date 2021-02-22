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
		// ˢ��ʱ��
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
		
			KFTimeData time_data;
			time_data._flag = xmlnode.ReadUInt32( "flag", true );
			time_data._month = xmlnode.ReadUInt32( "month", true );
			time_data._day = xmlnode.ReadUInt32( "day", true );
			time_data._day_of_week = xmlnode.ReadUInt32( "dayofweek", true );
			time_data._hour = xmlnode.ReadUInt32( "hour", true );
			time_data._minute = xmlnode.ReadUInt32( "minute", true );
			kfsetting->_time_data = time_data;
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
