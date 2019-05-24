#ifndef __KF_HERO_CONFIG_H__
#define __KF_HERO_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFHeroSetting
    {
    public:
        // 是否拥有皮肤
        bool HaveSkin( uint32 skinid ) const;
    public:

        // 英雄id
        uint32 _id{ 0 };

        // 品质
        uint32 _quality;

        // 默认皮肤
        uint32 _default_skin_id;

        // 皮肤id列表
        std::set< uint32 > _skin_list;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFHeroConfig : public KFConfig, public KFSingleton< KFHeroConfig >
    {
    public:
        KFHeroConfig() = default;
        ~KFHeroConfig() = default;

        // 查找配置
        const KFHeroSetting* FindHeroSetting( uint32 heroid ) const;

        // 读取配置
        bool LoadConfig( const std::string& file );
    private:
        // 英雄列表
        KFHashMap< uint32, uint32, KFHeroSetting > _hero_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_hero_config = KFHeroConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif