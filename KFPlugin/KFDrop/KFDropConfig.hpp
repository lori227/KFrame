#ifndef __KF_DROP_CONFIG_H__
#define __KF_DROP_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    namespace KFDropEnum
    {
        enum MyEnum
        {
            Rand = 1,		// 随机
            Weight = 2,		// 权重
        };
    }

    class KFDropData : public KFWeight
    {
    public:
        // 是否必掉
        bool _is_drop = false;

        // 掉落数据
        KFElements _data;
    };

    class KFDropSetting : public KFIntSetting
    {
    public:
        // 类型
        uint32 _type = 0;

        // 变量id
        uint32 _var_id = 0;

        // 最大次数
        uint32 _max_count = 0;

        // 掉落数据
        KFWeightList< KFDropData > _drop_list;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFDropConfig : public KFIntConfigT< KFDropSetting >, public KFInstance< KFDropConfig >
    {
    public:
        KFDropConfig()
        {
            _file_name = "drop";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFDropSetting* kfsetting );
    };
}

#endif