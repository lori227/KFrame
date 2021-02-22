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
            _file_name = "time_section";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFTimeSectionSetting* kfsetting )
        {
            KFTimeSection time_section;
            time_section._start_time_data._flag = xmlnode.ReadUInt32( "flag" );
            time_section._start_time_data._year = xmlnode.ReadUInt32( "startyear" );
            time_section._start_time_data._month = xmlnode.ReadUInt32( "startmonth" );
            time_section._start_time_data._day = xmlnode.ReadUInt32( "startDay" );
            time_section._start_time_data._day_of_week = xmlnode.ReadUInt32( "startdayofweek" );
            time_section._start_time_data._hour = xmlnode.ReadUInt32( "starthour" );
            time_section._start_time_data._minute = xmlnode.ReadUInt32( "startminute" );

            time_section._finish_time_data._flag = xmlnode.ReadUInt32( "flag" );
            time_section._finish_time_data._year = xmlnode.ReadUInt32( "finishyear" );
            time_section._finish_time_data._month = xmlnode.ReadUInt32( "finishmonth" );
            time_section._finish_time_data._day = xmlnode.ReadUInt32( "finishday" );
            time_section._finish_time_data._day_of_week = xmlnode.ReadUInt32( "finishdayofweek" );
            time_section._finish_time_data._hour = xmlnode.ReadUInt32( "finishhour" );
            time_section._finish_time_data._minute = xmlnode.ReadUInt32( "finishminute" );

            kfsetting->_time_section.emplace_back( time_section );
        }
    };
}

#endif