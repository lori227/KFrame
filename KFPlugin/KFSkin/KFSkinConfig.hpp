#ifndef __KF_SKIN_CONFIG_H__
#define __KF_SKIN_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    class KFSkinSetting : public KFIntSetting
    {
    public:
        // 品质
        uint32 _quality = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFSkinConfig : public KFConfigT< KFSkinSetting >, public KFInstance< KFSkinConfig >
    {
    public:
        KFSkinConfig()
        {
            _file_name = "skin";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xml_node, KFSkinSetting* kfsetting );
    };
}

#endif