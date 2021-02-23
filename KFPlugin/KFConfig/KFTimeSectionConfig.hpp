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

    class KFTimeSectionConfig : public KFConfigT<KFTimeSectionSetting>, public KFInstance<KFTimeSectionConfig>
    {
    public:
        KFTimeSectionConfig()
        {
            _key_name = "id";
            _file_name = "time_section";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFTimeSectionSetting> setting )
        {
            KFTimeSection time_section;
            time_section._start_time_data._flag = xml_node.ReadUInt32( "flag" );
            time_section._start_time_data._year = xml_node.ReadUInt32( "startyear" );
            time_section._start_time_data._month = xml_node.ReadUInt32( "startmonth" );
            time_section._start_time_data._day = xml_node.ReadUInt32( "startDay" );
            time_section._start_time_data._day_of_week = xml_node.ReadUInt32( "startdayofweek" );
            time_section._start_time_data._hour = xml_node.ReadUInt32( "starthour" );
            time_section._start_time_data._minute = xml_node.ReadUInt32( "startminute" );

            time_section._finish_time_data._flag = xml_node.ReadUInt32( "flag" );
            time_section._finish_time_data._year = xml_node.ReadUInt32( "finishyear" );
            time_section._finish_time_data._month = xml_node.ReadUInt32( "finishmonth" );
            time_section._finish_time_data._day = xml_node.ReadUInt32( "finishday" );
            time_section._finish_time_data._day_of_week = xml_node.ReadUInt32( "finishdayofweek" );
            time_section._finish_time_data._hour = xml_node.ReadUInt32( "finishhour" );
            time_section._finish_time_data._minute = xml_node.ReadUInt32( "finishminute" );

            setting->_time_section.emplace_back( time_section );
        }
    };
}

#endif