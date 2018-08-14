#ifndef __KF_ZONE_CONFIG_H__
#define __KF_ZONE_CONFIG_H__

#include "KFrame.h"
#include "KFZoneInterface.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFZoneConfig : public KFConfig, public KFSingleton< KFZoneConfig >
    {
    public:
        KFZoneConfig();
        ~KFZoneConfig();

        bool LoadConfig();

        // 获得zoneid
        const KFZone* FindZone( uint32 appflag, uint32 zoneid );

    public:
        // 分区列表
        typedef std::pair< uint32, uint32 > ZoneKey;
        KFMap < ZoneKey, const ZoneKey&, KFZone > _zone_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_zone_config = KFZoneConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif