#ifndef __KF_DROP_DATA_CONFIG_H__
#define __KF_DROP_DATA_CONFIG_H__

#include "KFConfig.h"
#include "KFXmlReader/KFDropData.h"
#include "KFXmlReader/KFElementConfig.h"

namespace KFrame
{
    class KFDropDataWeight : public KFWeight
    {
    public:
        // 掉落权重
        KFDropData _drop_data;
    };

    class KFDropDataSetting : public KFIntSetting
    {
    public:
        // 掉落数据
        KFWeightList< KFDropDataWeight > _drop_data_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    class KFDropDataConfig : public KFConfigT< KFDropDataSetting >, public KFInstance< KFDropDataConfig >
    {
    public:
        KFDropDataConfig()
        {
            _file_name = "dropdata";
        }

        virtual void LoadAllComplete();

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFDropDataSetting* kfsetting );
    };
}

#endif