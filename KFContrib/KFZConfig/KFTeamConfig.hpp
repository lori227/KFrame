#ifndef __KF_TEAM_CONFIG_H__
#define __KF_TEAM_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"

namespace KFrame
{
    class KFTeamSetting : public KFIntSetting
    {
    public:
        // 最大数量
        uint32 _max_count = 0u;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFTeamConfig : public KFConfigT< KFTeamSetting >, public KFInstance< KFTeamConfig >
    {
    public:
        KFTeamConfig()
        {
            _file_name = "team";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTeamSetting* kfsetting );
    };
}

#endif