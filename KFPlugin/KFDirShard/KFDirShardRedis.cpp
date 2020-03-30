#include "KFDirShardRedis.hpp"

namespace KFrame
{
#define __DIR_REDIS_DRIVER__ _kf_redis->Create( __STRING__( dir ) )

    bool KFDirShardRedis::ZoneRegister( uint32 zoneid, KFJson& zonedata )
    {
        StringMap values;
        __JSON_TO_MAP__( zonedata, values );

        // 先保存小区基本信息
        auto redisdriver = __DIR_REDIS_DRIVER__;
        redisdriver->Append( "zincrby {} 0 {}", __STRING__( zonelist ), zoneid );
        redisdriver->Append( values, "hmset {}:{}", __STRING__( zone ), zoneid );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    bool KFDirShardRedis::ZoneUpdate( uint64 appid, uint32 zoneid, uint32 count, const std::string& ip, uint32 port, uint32 expiretime )
    {
        // 判断最小负载
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfquery = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( gate ), zoneid );
        if ( !kfquery->_value.empty() )
        {
            auto lastappid = __TO_UINT64__( kfquery->_value[ __STRING__( appid ) ] );
            auto lastcount = __TO_UINT32__( kfquery->_value[ __STRING__( count ) ] );
            if ( lastappid != appid && count > lastcount )
            {
                return true;
            }
        }

        redisdriver->Append( "hmset {}:{} {} {} {} {} {} {} {} {}",
                             __STRING__( gate ), zoneid,
                             __STRING__( appid ), appid,
                             __STRING__( count ), count,
                             __STRING__( ip ), ip,
                             __STRING__( port ), port );
        redisdriver->Append( "expire {}:{} {}", __STRING__( gate ), zoneid, expiretime );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    StringListMap KFDirShardRedis::QueryZoneList()
    {
        StringListMap zonedatalist;
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfquerylist = redisdriver->QueryList( "zrange {} 0 -1", __STRING__( zonelist ) );
        for ( auto& strid : kfquerylist->_value )
        {
            auto zoneid = __TO_UINT32__( strid );
            auto zonedata = QueryZoneData( zoneid );
            if ( !zonedata.empty() )
            {
                zonedatalist.emplace_back( zonedata );
            }
        }

        return zonedatalist;
    }

    StringMap KFDirShardRedis::QueryZoneIp( uint32 zoneid )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( gate ), zoneid );
        return kfresult->_value;
    }

    StringMap KFDirShardRedis::QueryZoneData( uint32 zoneid )
    {
        StringMap zonedata;
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfgatedata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( gate ), zoneid );
        if ( kfgatedata->_value.empty() )
        {
            return zonedata;
        }

        auto kfzonedata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( zone ), zoneid );
        if ( kfzonedata->_value.empty() )
        {
            return zonedata;
        }

        zonedata = kfzonedata->_value;
        for ( auto& iter : kfgatedata->_value )
        {
            zonedata[ iter.first ] = iter.second;
        }
        return zonedata;
    }

    bool KFDirShardRedis::ZoneBalance( uint32 zoneid, uint32 count )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->Execute( "zincrby {} {} {}", __STRING__( zonelist ), count, zoneid );
        return kfresult->IsOk();
    }

    bool KFDirShardRedis::SetZoneRecommend( uint32 zoneid )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->Execute( "set {} {}", __STRING__( zonerecommend ), zoneid );
        return kfresult->IsOk();
    }

    uint32 KFDirShardRedis::GetZoneRecommend()
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryUInt64( "get {}", __STRING__( zonerecommend ) );
        return static_cast< uint32 >( kfresult->_value );
    }

    uint32 KFDirShardRedis::BalanceAllocZone()
    {
        // todo:如果有推荐的服务器, 直接返回
        auto zoneid = GetZoneRecommend();
        if ( zoneid != _invalid_int )
        {
            return zoneid;
        }

        // 选择一个最小人数的分区
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto zonelist = redisdriver->QueryList( "zrange {} 0 -1", __STRING__( zonelist ) );
        if ( zonelist->_value.empty() )
        {
            return _invalid_int;
        }

        for ( auto& strid : zonelist->_value )
        {
            auto kfqueryip = redisdriver->QueryString( "hget {}:{} {}", __STRING__( gate ), strid, __STRING__( ip ) );
            if ( !kfqueryip->_value.empty() )
            {
                return __TO_UINT32__( strid );
            }
        }

        // 找不到 直接返回1
        return 1u;
    }

    StringMap KFDirShardRedis::AllocPlayerZone( uint32 zoneid )
    {
        StringMap zonedata;
        if ( zoneid == _invalid_int )
        {
            zoneid = BalanceAllocZone();
            if ( zoneid == _invalid_int )
            {
                return zonedata;
            }
        }

        return QueryZoneData( zoneid );
    }

    bool KFDirShardRedis::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->Execute( "set {}:{} {}", __STRING__( worldurl ), worldid, url );
        return kfresult->IsOk();
    }

    std::string KFDirShardRedis::GetWorldUrl( uint64 worldid )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryString( "get {}:{}", __STRING__( worldurl ), worldid );
        return kfresult->_value;
    }
}