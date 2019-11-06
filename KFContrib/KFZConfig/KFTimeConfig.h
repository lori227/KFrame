#ifndef __KF_TIME_CONFIG_H__
#define __KF_TIME_CONFIG_H__

#include "KFConfig.h"
#include "KFSetting.h"

namespace KFrame
{
    class KFTimeSetting : public KFIntSetting
    {
    public:
        KFTimeData _time_data;
    };

    class KFTimeConfig : public KFConfigT< KFTimeSetting >, public KFInstance< KFTimeConfig >
    {
    public:
        KFTimeConfig()
        {
            _file_name = "time";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTimeSetting* kfsetting )
        {
            kfsetting->_time_data._type = xmlnode.GetUInt32( "Type" );
            kfsetting->_time_data._value = xmlnode.GetUInt32( "Value", true );
            kfsetting->_time_data._hour = xmlnode.GetUInt32( "Hour", true );
        }
    };
}

#endif