#ifndef __KF_ITEM_CONFIG_H__
#define __KF_ITEM_CONFIG_H__

#include "KFItemSetting.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFItemConfig : public KFIntConfigT< KFItemSetting >, public KFSingleton< KFItemConfig >
    {
    public:
        KFItemConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFItemSetting >( file, isclear )
        {
        }

        // 是否是配置文件
        virtual bool IsFile( const std::string& configfile, const std::string& file );

        // 加载完成
        virtual void LoadComplete( const std::string& file );
    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取通用道具
        void ReadCommonSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取礼包配置
        void ReadGiftSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取药品配置
        void ReadMedicineSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取装备配置
        void ReadEquipSetting( KFNode& xmlnode, KFItemSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_item_config = KFItemConfig::Instance( "item.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif