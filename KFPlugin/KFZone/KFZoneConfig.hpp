#ifndef __KF_ZONE_CONFIG_H__
#define __KF_ZONE_CONFIG_H__

#include "KFrame.h"
#include "KFZoneInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFZoneConfig : public KFConfig, public KFSingleton< KFZoneConfig >
    {
    public:
        KFZoneConfig( const std::string& file )
            : KFConfig( file )
        {
        }

        // 加载配置
        void LoadConfig( const std::string& file );

        // 查找小区信息
        const KFZone* FindZone( uint32 zoneid );

    protected:
        // 设置小区信息
        void SetZoneData( KFZone* zone, uint32 zoneid );

    public:
        // 本分区属性
        KFZone _zone;

        // 小区模板
        KFZone _zone_template;

        // 小区列表
        KFHashMap< uint32, uint32, KFZone > _zone_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_zone_config = KFZoneConfig::Instance( "zone.setting" );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif