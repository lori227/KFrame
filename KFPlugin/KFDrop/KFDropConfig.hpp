#ifndef __KF_DROP_CONFIG_H__
#define __KF_DROP_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFDropSetting.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    class KFDropConfig : public KFConfigT< KFDropSetting >, public KFInstance< KFDropConfig >
    {
    public:
        KFDropConfig()
        {
            _file_name = "drop";
        }

        // 全部读取
        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFDropSetting* kfsetting );

        // 格式化掉落配置
        void FormatDropElement( KFDropWeight* dropweight );
    };
}

#endif