#ifndef __KF_ITEM_CONFIG_H__
#define __KF_ITEM_CONFIG_H__

#include "KFItemSetting.hpp"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFItemConfig : public KFIntConfigT< KFItemSetting >, public KFInstance< KFItemConfig >
    {
    public:
        KFItemConfig()
        {
            _file_name = "item";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取通用道具
        void ReadCommonSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取礼包配置
        void ReadGiftSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取药品配置
        void ReadMedicineSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取装备配置
        void ReadEquipSetting( KFNode& xmlnode, KFItemSetting* kfsetting );
    };
}

#endif