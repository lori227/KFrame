#include "KFDirShardMongo.hpp"

namespace KFrame
{
#define __DIR_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( dir ) )

    bool KFDirShardMongo::ZoneRegister( uint32 zoneid, KFJson& zonedata )
    {
        return true;
    }

    bool KFDirShardMongo::ZoneUpdate( uint64 appid, uint32 zoneid, uint32 count, const std::string& ip, uint32 port, uint32 expiretime )
    {
        return true;
    }

    StringListMap KFDirShardMongo::QueryZoneList()
    {
        StringListMap zonedatalist;

        return zonedatalist;
    }

    StringMap KFDirShardMongo::QueryZoneIp( uint32 zoneid )
    {
        StringMap zonedata;
        return zonedata;
    }

    StringMap KFDirShardMongo::QueryZoneData( uint32 zoneid )
    {
        StringMap zonedata;
        return zonedata;
    }

    bool KFDirShardMongo::ZoneBalance( uint32 zoneid, uint32 count )
    {
        return true;
    }

    bool KFDirShardMongo::SetZoneRecommend( uint32 zoneid )
    {
        return true;
    }

    uint32 KFDirShardMongo::GetZoneRecommend()
    {
        return 0;
    }

    uint32 KFDirShardMongo::BalanceAllocZone()
    {
        // 找不到 直接返回1
        return 1u;
    }

    StringMap KFDirShardMongo::AllocPlayerZone( uint32 zoneid )
    {
        StringMap zonedata;
        return zonedata;
    }

    bool KFDirShardMongo::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        return true;
    }

    std::string KFDirShardMongo::GetWorldUrl( uint64 worldid )
    {
        return _invalid_string;
    }
}