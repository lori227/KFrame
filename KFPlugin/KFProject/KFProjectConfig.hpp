#ifndef __KF_PROJECT_CONFIG_H__
#define __KF_PROJECT_CONFIG_H__

#include "KFZConfig/KFSetting.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFProjectSetting : public KFStrSetting
    {
    public:
        // 整型数值
        uint64 _uint64_value = 0;

        // 浮点数值
        double _double_value = 0.0f;

        // 字符串数值
        std::string _str_value;
    };

    class KFProjectConfig : public KFConfigT< KFProjectSetting >, public KFInstance< KFProjectConfig >
    {
    public:
        KFProjectConfig()
        {
            _file_name = "project";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFProjectSetting* kfsetting );
    };
}

#endif