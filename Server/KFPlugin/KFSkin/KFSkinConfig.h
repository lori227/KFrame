#ifndef __KF_SKIN_CONFIG_H__
#define __KF_SKIN_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{

    ////////////////////////////////////////////////////////////////////////////////
    class KFSkinSetting
    {
    public:
        // 物品id
        uint32 _id{ 0 };

        // 品质
        uint32 _quality{ 0 };
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFSkinConfig : public KFConfig, public KFSingleton< KFSkinConfig >
    {
    public:
        KFSkinConfig() = default;
        ~KFSkinConfig() = default;

        // 读取配置
        bool LoadConfig();

        // 查找配置
        const KFSkinSetting* FindSkinSetting( uint32 id ) const;

    private:
        // 皮肤列表
        KFMap< uint32, uint32, KFSkinSetting > _skin_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_skin_config = KFSkinConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif