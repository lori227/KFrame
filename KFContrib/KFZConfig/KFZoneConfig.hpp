#ifndef __KF_ZONE_CONFIG_H__
#define __KF_ZONE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFZoneSetting.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFZoneConfig : public KFConfig, public KFInstance< KFZoneConfig >
    {
    public:
        KFZoneConfig()
        {
            _file_name = "zone";
        }

        // 加载配置
        bool LoadConfig( const std::string& file, uint32 loadmask );

        // 查找小区信息
        const KFZoneSetting* ZoneSetting();
        const KFZoneSetting* FindSetting( uint32 zoneid );

    protected:
        // 设置小区信息
        void SetZoneData( KFZoneSetting* kfsetting, uint32 zoneid );

    private:
        // 本分区属性
        KFZoneSetting _zone;

        // 小区模板
        KFZoneSetting _zone_template;

        // 小区列表
        KFHashMap< uint32, uint32, KFZoneSetting > _zone_list;
    };
}

#endif