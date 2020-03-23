#ifndef __KF_UNLOCK_CONFIG_H__
#define __KF_UNLOCK_CONFIG_H__

#include "KFCore/KFCondition.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFUnlockSetting : public KFIntSetting
    {
    public:
        // 属性
        std::string _data_name;

        // key
        uint32 _data_id;

        // 数值
        uint32 _data_value;

        // 子属性
        std::string _child_name;

        // 完成条件
        uint32 _condition_type = 0u;
        UInt32Vector _unlock_condition;
    };

    class KFUnlockConfig : public KFConfigT< KFUnlockSetting >, public KFInstance< KFUnlockConfig >
    {
    public:
        KFUnlockConfig()
        {
            _file_name = "unlock";
        }

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFUnlockSetting* kfsetting );
    };
}

#endif