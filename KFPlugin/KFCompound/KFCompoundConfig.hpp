#ifndef __KF_COMPOUND_CONFIG_H__
#define __KF_COMPOUND_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFCompoundSetting : public KFIntSetting
    {
    public:
        // 属性名字
        std::string _data_name;

        // 属性id
        uint32 _key = 0u;

        // 合成的属性
        KFElements _compound_data;

        // 花费材料
        KFElements _cost_data;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFCompoundConfig : public KFConfigT< KFCompoundSetting >, public KFInstance< KFCompoundConfig >
    {
    public:
        KFCompoundConfig()
        {
            _file_name = "compound";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFCompoundSetting* kfsetting );
    };
}

#endif