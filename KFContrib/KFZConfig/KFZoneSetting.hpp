#ifndef __KF_ZONE_SETTING_H__
#define __KF_ZONE_SETTING_H__

#include "KFZConfig/KFSetting.h"

namespace KFrame
{
    // 分区设置
    class KFZoneSetting : public KFIntSetting
    {
    public:
        // 游戏服名字
        std::string _name;

        // 逻辑id
        uint32 _logic_id = 0u;
    };
}

#endif