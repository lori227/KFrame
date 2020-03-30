#ifndef __KF_DIR_SHARD_MONGO_H__
#define __KF_DIR_SHARD_MONGO_H__

#include "KFDirShardLogic.hpp"
#include "KFMongo/KFMongoInterface.h"

namespace KFrame
{
    class KFDirShardMongo : public KFDirShardLogic
    {
    public:
        KFDirShardMongo() = default;
        ~KFDirShardMongo() = default;

        // 注册小区信息
        virtual bool ZoneRegister( uint32 zoneid, KFJson& zonedata );

        // 更新小区gate在线人数
        virtual bool ZoneUpdate( uint64 appid, uint32 zoneid, uint32 count, const std::string& ip, uint32 port, uint32 expiretime );

        // 查询小区列表
        virtual StringListMap QueryZoneList();

        // 查询小区ip
        virtual StringMap QueryZoneIp( uint32 zoneid );

        // 小区人数负载均衡
        virtual bool ZoneBalance( uint32 zoneid, uint32 count );

        // 分配玩家小区
        virtual StringMap AllocPlayerZone( uint32 zoneid );

        // 设置推荐小区
        virtual bool SetZoneRecommend( uint32 zoneid );

        // world http地址注册
        virtual bool SetWorldUrl( uint64 worldid, const std::string& url );

        // 获得world http地址
        virtual std::string GetWorldUrl( uint64 worldid );
    public:
        // 分配一个小区
        virtual uint32 BalanceAllocZone();

        // 查询推荐小区
        virtual uint32 GetZoneRecommend();

        // 查询小区信息
        virtual StringMap QueryZoneData( uint32 zoneid );
    };
}



#endif