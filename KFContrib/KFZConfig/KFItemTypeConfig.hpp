#ifndef __KF_ITEM_TYPE_CONFIG_H__
#define __KF_ITEM_TYPE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFItemTypeSeting : public KFIntSetting
    {
    public:
        // 仓库名字
        std::string _store_name;

        // 背包名字
        std::string _bag_name;

        // 额外的背包名
        std::string _extend_name;

        // 移动目标名字列表
        std::set< std::string > _move_name_list;

        // 是否自动放入仓库
        bool _is_auto_store = false;

        // 使用限制
        uint32 _use_limit = 0u;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFItemTypeConfig : public KFConfigT< KFItemTypeSeting >, public KFInstance< KFItemTypeConfig >
    {
    public:
        KFItemTypeConfig()
        {
            _file_name = "itemtype";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFItemTypeSeting* kfsetting );
    };
}

#endif