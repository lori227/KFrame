#include "KFDirAttributeRedis.hpp"

namespace KFrame
{
#define __DIR_REDIS_DRIVER__ _kf_redis->Create( __STRING__( dir ) )

    bool KFDirAttributeRedis::ZoneRegister( uint32 zoneid, KFJson& zonedata )
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

    bool KFDirAttributeRedis::ZoneUpdate( uint64 appid, uint32 zoneid, uint32 count, const std::string& ip, uint32 port, uint32 expiretime )
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

    uint32 KFDirAttributeRedis::QueryZoneStatus( uint32 zoneid )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( zone ), zoneid, __STRING__( status ) );
        return kfresult->_value;
    }

    bool KFDirAttributeRedis::UpdateZoneStatus( uint32 zoneid, uint32 status )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        if ( zoneid != 0u )
        {
            redisdriver->Append( "hset {}:{} {} {}", __STRING__( zone ), zoneid, __STRING__( status ), status );
        }
        else
        {
            auto kflist = redisdriver->QueryList( "zrange {} 0 -1", __STRING__( zonelist ) );
            for ( auto& strzoneid : kflist->_value )
            {
                redisdriver->Append( "hset {}:{} {} {}", __STRING__( zone ), strzoneid, __STRING__( status ), status );
            }
        }

        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    StringMapList KFDirAttributeRedis::QueryZoneList( const std::string& flag )
    {
        StringMapList zonedatalist;
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfquerylist = redisdriver->QueryList( "zrange {} 0 -1", __STRING__( zonelist ) );
        for ( auto& strid : kfquerylist->_value )
        {
            auto zoneid = __TO_UINT32__( strid );
            auto zonedata = QueryZoneData( zoneid );
            if ( !zonedata.empty() )
            {
                if ( flag.empty() || flag == zonedata[ __STRING__( flag ) ] )
                {
                    zonedatalist.emplace_back( zonedata );
                }
            }
        }

        return zonedatalist;
    }

    StringMap KFDirAttributeRedis::QueryZoneIp( uint32 zoneid )
    {
        StringMap zonedata;
        auto redisdriver = __DIR_REDIS_DRIVER__;

        auto kfqueryloginid = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( zone ), zoneid );
        if ( kfqueryloginid->_value == 0u )
        {
            return zonedata;
        }

        auto kfresult = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( gate ), kfqueryloginid->_value );
        zonedata.swap( kfresult->_value );
        return zonedata;
    }

    StringMap KFDirAttributeRedis::QueryZoneData( uint32 zoneid )
    {
        StringMap zonedata;
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfzonedata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( zone ), zoneid );
        if ( kfzonedata->_value.empty() )
        {
            return zonedata;
        }

        auto loginzoneid = kfzonedata->_value[ __STRING__( loginzoneid ) ];
        auto kfgatedata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( gate ), loginzoneid );
        if ( kfgatedata->_value.empty() )
        {
            return zonedata;
        }

        zonedata = kfgatedata->_value;
        for ( auto& iter : kfzonedata->_value )
        {
            zonedata[ iter.first ] = iter.second;
        }
        return zonedata;
    }

    bool KFDirAttributeRedis::ZoneBalance( uint32 zoneid, uint32 count )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->Execute( "zincrby {} {} {}", __STRING__( zonelist ), count, zoneid );
        return kfresult->IsOk();
    }

    bool KFDirAttributeRedis::SetZoneRecommend( const std::string& flag, uint32 zoneid, bool isrecommend )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;

        auto ok = false;
        if ( isrecommend )
        {
            auto kfresult = redisdriver->Execute( "sadd {}:{} {}", __STRING__( zonerecommendlist ), flag, zoneid );
            ok = kfresult->IsOk();
        }
        else
        {
            auto kfresult = redisdriver->Execute( "srem {}:{} {}", __STRING__( zonerecommendlist ), flag, zoneid );
            ok = kfresult->IsOk();
        }

        return ok;
    }

    uint32 KFDirAttributeRedis::GetZoneRecommend( const std::string& flag )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryUInt64( "srandmember {}:{}", __STRING__( zonerecommendlist ), flag );
        return static_cast< uint32 >( kfresult->_value );
    }

    uint32 KFDirAttributeRedis::BalanceAllocZone( const std::string& flag )
    {
        // todo:如果有推荐的服务器, 直接返回
        auto zoneid = GetZoneRecommend( flag );
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
            auto kfloginzoneid = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( zone ), strid, __STRING__( loginzoneid ) );
            if ( kfloginzoneid->_value == 0u )
            {
                continue;
            }

            auto kfqueryip = redisdriver->QueryString( "hget {}:{} {}", __STRING__( gate ), kfloginzoneid->_value, __STRING__( ip ) );
            if ( !kfqueryip->_value.empty() )
            {
                return __TO_UINT32__( strid );
            }
        }

        // 找不到 直接返回1
        return 1u;
    }

    StringMap KFDirAttributeRedis::AllocPlayerZone( const std::string& flag, uint32 zoneid )
    {
        StringMap zonedata;
        if ( zoneid == _invalid_int )
        {
            zoneid = BalanceAllocZone( flag );
            if ( zoneid == _invalid_int )
            {
                return zonedata;
            }
        }

        return QueryZoneData( zoneid );
    }

    bool KFDirAttributeRedis::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->Execute( "set {}:{} {}", __STRING__( worldurl ), worldid, url );
        return kfresult->IsOk();
    }

    std::string KFDirAttributeRedis::GetWorldUrl( uint64 worldid )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryString( "get {}:{}", __STRING__( worldurl ), worldid );
        return kfresult->_value;
    }

    // 更新masterip
    bool KFDirAttributeRedis::UpdateMasterIp( const std::string& appname, uint64 appid, uint32 zoneid, const std::string& ip, uint32 port, uint32 expiretime )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;

        redisdriver->Append( "sadd {}:{}:{} {}", __STRING__( master ), appname, zoneid, appid );
        redisdriver->Append( "hmset {}:{} {} {} {} {} {} {}", __STRING__( master ), appid,
                             __STRING__( appid ), appid, __STRING__( ip ), ip, __STRING__( port ), port );
        redisdriver->Append( "expire {}:{} {}", __STRING__( master ), appid, expiretime );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    StringMap KFDirAttributeRedis::QueryMasterIp( const std::string& appname, uint32 zoneid )
    {
        StringMap zonedata;
        auto redisdriver = __DIR_REDIS_DRIVER__;

        auto kfappid = redisdriver->QueryString( "srandmember {}:{}:{}", __STRING__( master ), appname, zoneid );
        if ( !kfappid->_value.empty() )
        {
            auto kfmasterdata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( master ), kfappid->_value );
            if ( !kfmasterdata->_value.empty() )
            {
                zonedata.swap( kfmasterdata->_value );
            }
            else
            {
                if ( kfmasterdata->IsOk() )
                {
                    // 执行正常, 说明master数据过期了
                    redisdriver->Execute( "srem {}:{}:{} {}", __STRING__( master ), appname, zoneid, kfappid->_value );
                }
            }
        }

        return zonedata;
    }

    StringMapList KFDirAttributeRedis::QueryMasterList( const std::string& appname, uint32 zoneid )
    {
        StringMapList masterdatalist;
        auto redisdriver = __DIR_REDIS_DRIVER__;

        auto kfappidlist = redisdriver->QueryList( "smembers {}:{}:{}", __STRING__( master ), appname, zoneid );
        for ( auto& appid : kfappidlist->_value )
        {
            auto kfmasterdata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( master ), appid );
            if ( !kfmasterdata->_value.empty() )
            {
                StringMap masterdata;
                masterdata.swap( kfmasterdata->_value );
                masterdatalist.emplace_back( masterdata );
            }
            else
            {
                if ( kfmasterdata->IsOk() )
                {
                    // 执行正常, 说明master数据过期了
                    redisdriver->Execute( "srem {}:{}:{} {}", __STRING__( master ), appname, zoneid, appid );
                }
            }
        }

        return masterdatalist;
    }
}