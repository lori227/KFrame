#ifndef __KF_HERO_CONFIG_H__
#define __KF_HERO_CONFIG_H__

#include "KFrame.h"

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
    class KFHeroConfig : public KFIntConfigT< KFHeroSetting >, public KFSingleton< KFHeroConfig >
    {
    public:
        KFHeroConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFHeroSetting >( file, isclear )
        {
        }

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFHeroSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_hero_config = KFHeroConfig::Instance( "hero.xml", true );
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif