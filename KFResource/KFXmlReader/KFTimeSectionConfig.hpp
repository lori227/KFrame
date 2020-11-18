#ifndef __KF_TIME_SECTION_CONFIG_H__
#define __KF_TIME_SECTION_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFTimeSectionSetting : public KFIntSetting
    {
    public:
        // 时间段列表
        std::vector< KFTimeSection > _time_section;
    };

    class KFTimeSectionConfig : public KFConfigT< KFTimeSectionSetting >, public KFInstance< KFTimeSectionConfig >
    {
    public:
        KFTimeSectionConfig()
        {
            _key_name = "id";
            _file_name = "timesection";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFTimeSectionSetting* kfsetting )
        {
            KFTimeSection timesection;
            timesection._start_time_data._flag = xmlnode.ReadUInt32( "flag" );
            timesection._start_time_data._year = xmlnode.ReadUInt32( "startyear" );
            timesection._start_time_data._month = xmlnode.ReadUInt32( "startmonth" );
            timesection._start_time_data._day = xmlnode.ReadUInt32( "startDay" );
            timesection._start_time_data._day_of_week = xmlnode.ReadUInt32( "startdayofweek" );
            timesection._start_time_data._hour = xmlnode.ReadUInt32( "starthour" );
            timesection._start_time_data._minute = xmlnode.ReadUInt32( "startminute" );

            timesection._finish_time_data._flag = xmlnode.ReadUInt32( "flag" );
            timesection._finish_time_data._year = xmlnode.ReadUInt32( "finishyear" );
            timesection._finish_time_data._month = xmlnode.ReadUInt32( "finishmonth" );
            timesection._finish_time_data._day = xmlnode.ReadUInt32( "finishday" );
            timesection._finish_time_data._day_of_week = xmlnode.ReadUInt32( "finishdayofweek" );
            timesection._finish_time_data._hour = xmlnode.ReadUInt32( "finishhour" );
            timesection._finish_time_data._minute = xmlnode.ReadUInt32( "finishminute" );

            kfsetting->_time_section.emplace_back( timesection );
        }
    };
}

#endif