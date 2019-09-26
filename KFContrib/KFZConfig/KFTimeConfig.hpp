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
        virtual void ReadSetting( KFNode& xmlnode, KFTimeSetting* kfsetting );
    };
}

#endif