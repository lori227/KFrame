#ifndef __KF_HERO_CONFIG_H__
#define __KF_HERO_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFHeroSetting : public KFIntSetting
    {
    public:
        // 是否拥有皮肤
        bool HaveSkin( uint32 skinid ) const;
    public:
        // 品质
        uint32 _quality;

        // 默认皮肤
        uint32 _default_skin_id;

        // 皮肤id列表
        std::set< uint32 > _skin_list;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFHeroConfig : public KFConfigT< KFHeroSetting >, public KFInstance< KFHeroConfig >
    {
    public:
        KFHeroConfig()
        {
            _file_name = "hero";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFHeroSetting* kfsetting );
    };
}

#endif