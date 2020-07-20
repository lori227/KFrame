#include "KFDirDatabaseRedis.hpp"

namespace KFrame
{
#define __DIR_REDIS_DRIVER__ _kf_redis->Create( __STRING__( dir ) )

    bool KFDirDatabaseRedis::ZoneRegister( uint32 zoneid, KFJson& zonedata )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;

        StringMap values;
        __JSON_TO_MAP__( zonedata, values );

        redisdriver->WriteMulti();
        redisdriver->ZIncrby( __STRING__( zonelist ), zoneid, 0 );
        redisdriver->HMSet( __DATABASE_KEY_2__(  __STRING__( zone ), zoneid ), values );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    bool KFDirDatabaseRedis::ZoneUpdate( uint64 appid, uint32 zoneid, uint32 count, const std::string& ip, uint32 port, uint32 expiretime )
    {
        // 判断最小负载
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto strkey = __DATABASE_KEY_2__( __STRING__( gate ), zoneid );

        auto kfquery = redisdriver->HGetAll( strkey );
        if ( !kfquery->_value.empty() )
        {
            auto lastappid = __TO_UINT64__( kfquery->_value[ __STRING__( appid ) ] );
            auto lastcount = __TO_UINT32__( kfquery->_value[ __STRING__( count ) ] );
            if ( lastappid != appid && count > lastcount )
            {
                return true;
            }
        }

        StringMap values;
        values[ ip ] = ip;
        values[ __STRING__( port ) ] = __TO_STRING__( port );
        values[ __STRING__( count ) ] = __TO_STRING__( count );
        values[ __STRING__( appid ) ] = __TO_STRING__( appid );

        redisdriver->WriteMulti();
        redisdriver->HMSet( strkey, values );
        redisdriver->Expire( strkey, expiretime );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    uint32 KFDirDatabaseRedis::QueryZoneStatus( uint32 zoneid )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( zone ), zoneid ), __STRING__( status ) );
        return kfresult->_value;
    }

    bool KFDirDatabaseRedis::UpdateZoneStatus( uint32 zoneid, uint32 status )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        if ( zoneid != 0u )
        {
            auto kfresult = redisdriver->HSet( __DATABASE_KEY_2__( __STRING__( zone ), zoneid ), __STRING__( status ), status );
            return kfresult->IsOk();
        }

        auto kflist = redisdriver->ZRange( __STRING__( zonelist ), 0, -1 );
        if ( kflist->_value.empty() )
        {
            return false;
        }

        redisdriver->WriteMulti();
        for ( auto& zonepair : kflist->_value )
        {
            redisdriver->HSet( __DATABASE_KEY_2__( __STRING__( zone ), zonepair.first ), __STRING__( status ), status );
        }
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    StringMapList KFDirDatabaseRedis::QueryZoneList( const std::string& flag )
    {
        StringMapList zonedatalist;
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfquerylist = redisdriver->ZRange( __STRING__( zonelist ), 0, -1 );
        for ( auto& zonepair : kfquerylist->_value )
        {
            auto zoneid = __TO_UINT32__( zonepair.first );
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

    StringMap KFDirDatabaseRedis::QueryZoneIp( uint32 zoneid )
    {
        StringMap zonedata;
        auto redisdriver = __DIR_REDIS_DRIVER__;

        auto kfqueryloginid = redisdriver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( zone ), zoneid ), __STRING__( loginzoneid ) );
        if ( kfqueryloginid->_value == 0u )
        {
            return zonedata;
        }

        auto kfresult = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( gate ), kfqueryloginid->_value ) );
        zonedata.swap( kfresult->_value );
        return zonedata;
    }

    StringMap KFDirDatabaseRedis::QueryZoneData( uint32 zoneid )
    {
        StringMap zonedata;
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfzonedata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( zone ), zoneid ) );
        if ( kfzonedata->_value.empty() )
        {
            return zonedata;
        }

        auto loginzoneid = kfzonedata->_value[ __STRING__( loginzoneid ) ];
        auto kfgatedata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( gate ), loginzoneid ) );
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

    bool KFDirDatabaseRedis::ZoneBalance( uint32 zoneid, uint32 count )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->ZIncrby( __STRING__( zonelist ), zoneid, count );
        return kfresult->IsOk();
    }

    bool KFDirDatabaseRedis::SetZoneRecommend( const std::string& flag, uint32 zoneid, bool isrecommend )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;

        auto ok = false;
        if ( isrecommend )
        {
            auto kfresult = redisdriver->SAdd(  __DATABASE_KEY_2__( __STRING__( zonerecommendlist ), flag ), zoneid );
            ok = kfresult->IsOk();
        }
        else
        {
            auto kfresult = redisdriver->SRem( __DATABASE_KEY_2__( __STRING__( zonerecommendlist ), flag ), zoneid );
            ok = kfresult->IsOk();
        }

        return ok;
    }

    uint32 KFDirDatabaseRedis::GetZoneRecommend( const std::string& flag )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->SRandMember( __DATABASE_KEY_2__( __STRING__( zonerecommendlist ), flag ) );
        return KFUtility::ToValue<uint32>( kfresult->_value );
    }

    uint32 KFDirDatabaseRedis::BalanceAllocZone( const std::string& flag )
    {
        // todo:如果有推荐的服务器, 直接返回
        auto zoneid = GetZoneRecommend( flag );
        if ( zoneid != _invalid_int )
        {
            return zoneid;
        }

        // 选择一个最小人数的分区
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto zonelist = redisdriver->ZRange( __STRING__( zonelist ), 0, -1 );
        if ( zonelist->_value.empty() )
        {
            return _invalid_int;
        }

        for ( auto& zonepair : zonelist->_value )
        {
            auto kfloginzoneid = redisdriver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( zone ), zonepair.first ), __STRING__( loginzoneid ) );
            if ( kfloginzoneid->_value == 0u )
            {
                continue;
            }

            auto kfqueryip = redisdriver->HGet( __DATABASE_KEY_2__( __STRING__( gate ), kfloginzoneid->_value ), __STRING__( ip ) );
            if ( !kfqueryip->_value.empty() )
            {
                return __TO_UINT32__( zonepair.first );
            }
        }

        // 找不到 直接返回1
        return 1u;
    }

    StringMap KFDirDatabaseRedis::AllocPlayerZone( const std::string& flag, uint32 zoneid )
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

    bool KFDirDatabaseRedis::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->Set( __DATABASE_KEY_2__( __STRING__( worldurl ), worldid ), url );
        return kfresult->IsOk();
    }

    std::string KFDirDatabaseRedis::GetWorldUrl( uint64 worldid )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto kfresult = redisdriver->Get( __DATABASE_KEY_2__( __STRING__( worldurl ), worldid ) );
        return kfresult->_value;
    }

    // 更新masterip
    bool KFDirDatabaseRedis::UpdateMasterIp( const std::string& appname, uint64 appid, uint32 zoneid, const std::string& ip, uint32 port, uint32 expiretime )
    {
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto strmasterkey = __DATABASE_KEY_2__( __STRING__( master ), appid );

        StringMap values;
        values[ __STRING__( ip ) ] = ip;
        values[ __STRING__( appid ) ] = __TO_STRING__( appid );
        values[ __STRING__( port ) ] = __TO_STRING__( port );

        redisdriver->WriteMulti();
        redisdriver->HMSet( strmasterkey, values );
        redisdriver->Expire( strmasterkey, expiretime );
        redisdriver->SAdd( __DATABASE_KEY_3__( __STRING__( master ), appname, zoneid ), appid );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    StringMap KFDirDatabaseRedis::QueryMasterIp( const std::string& appname, uint32 zoneid )
    {
        StringMap zonedata;
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto strmasternamekey = __DATABASE_KEY_3__( __STRING__( master ), appname, zoneid );

        auto kfappid = redisdriver->SRandMember( strmasternamekey );
        if ( !kfappid->_value.empty() )
        {
            auto kfmasterdata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( master ), kfappid->_value ) );
            if ( !kfmasterdata->_value.empty() )
            {
                zonedata.swap( kfmasterdata->_value );
            }
            else
            {
                if ( kfmasterdata->IsOk() )
                {
                    // 执行正常, 说明master数据过期了
                    redisdriver->SRem( strmasternamekey, kfappid->_value );
                }
            }
        }

        return zonedata;
    }

    StringMapList KFDirDatabaseRedis::QueryMasterList( const std::string& appname, uint32 zoneid )
    {
        StringMapList masterdatalist;
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto strmasternamekey = __DATABASE_KEY_3__( __STRING__( master ), appname, zoneid );

        auto kfappidlist = redisdriver->SMembers( strmasternamekey );
        for ( auto& appid : kfappidlist->_value )
        {
            auto kfmasterdata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( master ), appid ) );
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
                    redisdriver->SRem( strmasternamekey, appid );
                }
            }
        }

        return masterdatalist;
    }
}