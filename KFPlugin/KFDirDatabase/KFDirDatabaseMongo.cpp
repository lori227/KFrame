#include "KFDirDatabaseMongo.hpp"

namespace KFrame
{
#define __DIR_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( dir ) )

    bool KFDirDatabaseMongo::ZoneRegister( uint32 zone_id, KFJson& zonedata )
    {
        // 先保存小区基本信息
        auto mongodriver = __DIR_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( gate ), MongoKeyword::_expire );

        KFDBValue dbvalue;
        __JSON_TO_DBVALUE__( zonedata, dbvalue );
        return mongodriver->Insert( __STRING__( zone ), zone_id, dbvalue );
    }

    bool KFDirDatabaseMongo::ZoneUpdate( uint64 appid, uint32 zone_id, uint32 count, const std::string& ip, uint32 port, uint32 expiretime )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( appid ), appid );
        dbvalue.AddValue( __STRING__( ip ), ip );
        dbvalue.AddValue( __STRING__( port ), port );
        dbvalue.AddValue( __STRING__( appid ), appid );
        dbvalue.AddValue( MongoKeyword::_expire, expiretime );
        return mongodriver->Insert( __STRING__( gate ), zone_id, dbvalue );
    }

    uint32 KFDirDatabaseMongo::QueryZoneStatus( uint32 zone_id )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __STRING__( zone ), zone_id, __STRING__( status ) );
        return kfresult->_value;
    }

    bool KFDirDatabaseMongo::UpdateZoneStatus( uint32 zone_id, uint32 status )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        if ( zone_id != 0u )
        {
            return mongodriver->Insert( __STRING__( zone ), zone_id, __STRING__( status ), status );
        }

        return mongodriver->Insert( __STRING__( zone ), __STRING__( status ), status );
    }

    StringMapList KFDirDatabaseMongo::QueryZoneList( const std::string& flag )
    {
        StringMapList zonedatalist;
        auto mongodriver = __DIR_MONGO_DRIVER__;

        auto kfresult = mongodriver->QueryListRecord( __STRING__( zone ) );
        for ( auto& dbvalue : kfresult->_value )
        {
            auto zone_id = dbvalue.FindValue( __STRING__( zone_id ) );
            auto zonedata = QueryZoneData( zone_id );
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

    StringMap KFDirDatabaseMongo::QueryZoneIp( uint32 zone_id )
    {
        StringMap values;
        auto mongodriver = __DIR_MONGO_DRIVER__;

        auto kfloginid = mongodriver->QueryUInt64( __STRING__( zone ), zone_id, __STRING__( loginzoneid ) );
        if ( kfloginid->_value == 0u )
        {
            return values;
        }

        auto kfipdata = mongodriver->QueryRecord( __STRING__( gate ), kfloginid->_value );
        __DBVALUE_TO_MAP__( kfipdata->_value, values );
        return values;
    }

    StringMap KFDirDatabaseMongo::QueryZoneData( uint32 zone_id )
    {
        StringMap zonedata;
        auto mongodriver = __DIR_MONGO_DRIVER__;

        auto kfzonedata = mongodriver->QueryRecord( __STRING__( zone ), zone_id );
        if ( kfzonedata->_value.IsEmpty() )
        {
            return zonedata;
        }

        auto loginzoneid = kfzonedata->_value.FindValue( __STRING__( loginzoneid ) );
        auto kfgatedata = mongodriver->QueryRecord( __STRING__( gate ), loginzoneid );
        if ( kfgatedata->_value.IsEmpty() )
        {
            return zonedata;
        }

        __DBVALUE_TO_MAP__( kfzonedata->_value, zonedata );
        __DBVALUE_TO_MAP__( kfgatedata->_value, zonedata );
        return zonedata;
    }

    bool KFDirDatabaseMongo::ZoneBalance( uint32 zone_id, uint32 count )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        return mongodriver->Operate( __STRING__( zone ), zone_id, __STRING__( count ), KFEnum::Add, count );
    }

    bool KFDirDatabaseMongo::SetZoneRecommend( const std::string& flag, uint32 zone_id, bool isrecommend )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;

        auto ok = false;
        if ( isrecommend )
        {
            ok = mongodriver->Push( __STRING__( zonerecommendlist ), flag, __STRING__( zone_id ), zone_id );

        }
        else
        {
            ok = mongodriver->Pull( __STRING__( zonerecommendlist ), flag, __STRING__( zone_id ), zone_id );
        }

        return ok;
    }

    uint32 KFDirDatabaseMongo::GetZoneRecommend( const std::string& flag )
    {
        auto zone_id = 0u;
        auto mongodriver = __DIR_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListUInt64( __STRING__( zonerecommendlist ), flag, __STRING__( zone_id ) );
        if ( kfresult->_value.empty() )
        {
            auto size = ( uint32 )kfresult->_value.size();
            auto index = rand() % size;
            auto iter = kfresult->_value.begin();
            std::advance( iter, index );
            zone_id = *iter;
        }

        return zone_id;
    }

    uint32 KFDirDatabaseMongo::BalanceAllocZone( const std::string& flag )
    {
        auto zone_id = GetZoneRecommend( flag );
        if ( zone_id != _invalid_int )
        {
            return zone_id;
        }
        // 找一个人数最少的区
        auto mongodriver = __DIR_MONGO_DRIVER__;

        // 数量升序显示
        KFMongoSelector kfselector;
        kfselector.AddReturn( __STRING__( zone_id ), MongoKeyword::_asc );
        kfselector.AddReturn( __STRING__( count ), MongoKeyword::_asc );
        auto kfresult = mongodriver->QueryListRecord( __STRING__( zone ), kfselector );
        for ( auto& dbvalue : kfresult->_value )
        {
            auto zone_id = dbvalue.FindValue( __STRING__( zone_id ) );
            auto kfrecord = mongodriver->QueryRecord( __STRING__( gate ), zone_id );
            if ( !kfrecord->_value.IsEmpty() )
            {
                return zone_id;
            }
        }

        return 1u;
    }

    StringMap KFDirDatabaseMongo::AllocPlayerZone( const std::string& flag, uint32 zone_id )
    {
        StringMap zonedata;
        if ( zone_id == _invalid_int )
        {
            zone_id = BalanceAllocZone( flag );
            if ( zone_id == _invalid_int )
            {
                return zonedata;
            }
        }

        return QueryZoneData( zone_id );
    }

    bool KFDirDatabaseMongo::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        return mongodriver->Insert( __STRING__( worldurl ), worldid, __STRING__( url ), url );
    }

    std::string KFDirDatabaseMongo::GetWorldUrl( uint64 worldid )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryString( __STRING__( worldurl ), worldid, __STRING__( url ) );
        return kfresult->_value;
    }

    // 更新masterip
    bool KFDirDatabaseMongo::UpdateMasterIp( const std::string& appname, uint64 appid, uint32 zone_id, const std::string& ip, uint32 port, uint32 expiretime )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( master ), MongoKeyword::_expire );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( appname ), appname );
        dbvalue.AddValue( __STRING__( appid ), appid );
        dbvalue.AddValue( __STRING__( zone_id ), zone_id );
        dbvalue.AddValue( __STRING__( ip ), ip );
        dbvalue.AddValue( __STRING__( port ), port );
        dbvalue.AddValue( MongoKeyword::_expire, expiretime );
        return mongodriver->Insert( __STRING__( master ), appid, dbvalue );
    }

    StringMap KFDirDatabaseMongo::QueryMasterIp( const std::string& appname, uint32 zone_id )
    {
        StringMap masterdata;
        auto mongodriver = __DIR_MONGO_DRIVER__;

        KFMongoSelector kfselector;
        kfselector.AddLimitReturn( MongoKeyword::_id );
        kfselector._document.AddExpression( __STRING__( appname ), MongoKeyword::_eq, appname );
        kfselector._document.AddExpression( __STRING__( zone_id ), MongoKeyword::_eq, zone_id );
        auto kfresult = mongodriver->QueryListRecord( __STRING__( master ), kfselector );
        if ( !kfresult->_value.empty() )
        {
            auto size = ( uint32 )kfresult->_value.size();
            auto index = rand() % size;
            auto iter = kfresult->_value.begin();
            std::advance( iter, index );

            auto& dbvalue = *iter;
            __DBVALUE_TO_MAP__( dbvalue, masterdata );
        }

        return masterdata;
    }

    StringMapList KFDirDatabaseMongo::QueryMasterList( const std::string& appname, uint32 zone_id )
    {
        StringMapList masterdatalist;
        auto mongodriver = __DIR_MONGO_DRIVER__;

        KFMongoSelector kfselector;
        kfselector.AddLimitReturn( MongoKeyword::_id );
        kfselector._document.AddExpression( __STRING__( appname ), MongoKeyword::_eq, appname );
        kfselector._document.AddExpression( __STRING__( zone_id ), MongoKeyword::_eq, zone_id );
        auto kfresult = mongodriver->QueryListRecord( __STRING__( master ), kfselector );
        for ( auto& dbvalue : kfresult->_value )
        {
            StringMap masterdata;
            __DBVALUE_TO_MAP__( dbvalue, masterdata );
            masterdatalist.emplace_back( masterdata );
        }

        return masterdatalist;
    }
}