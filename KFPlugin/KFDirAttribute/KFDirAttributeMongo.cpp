#include "KFDirAttributeMongo.hpp"

namespace KFrame
{
#define __DIR_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( dir ) )

    bool KFDirAttributeMongo::ZoneRegister( uint32 zoneid, KFJson& zonedata )
    {
        // 先保存小区基本信息
        auto mongodriver = __DIR_MONGO_DRIVER__;

        KFDBValue dbvalue;
        __JSON_TO_DBVALUE__( zonedata, dbvalue );
        return mongodriver->Insert( __STRING__( zone ), zoneid, dbvalue );
    }

    bool KFDirAttributeMongo::ZoneUpdate( uint64 appid, uint32 zoneid, uint32 count, const std::string& ip, uint32 port, uint32 expiretime )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( appid ), appid );
        dbvalue.AddValue( __STRING__( ip ), ip );
        dbvalue.AddValue( __STRING__( port ), port );
        dbvalue.AddValue( __STRING__( appid ), appid );
        dbvalue.AddValue( MongoKeyword::_expire, expiretime );
        return mongodriver->Insert( __STRING__( gate ), zoneid, dbvalue );
    }

    StringListMap KFDirAttributeMongo::QueryZoneList()
    {
        StringListMap zonedatalist;
        auto mongodriver = __DIR_MONGO_DRIVER__;

        KFMongoSelector kfselector;
        auto kfresult = mongodriver->QueryListRecord( __STRING__( zone ), kfselector );
        for ( auto& dbvalue : kfresult->_value )
        {
            auto zoneid = dbvalue.FindValue( __STRING__( zoneid ) );
            auto zonedata = QueryZoneData( zoneid );
            if ( !zonedata.empty() )
            {
                zonedatalist.emplace_back( zonedata );
            }
        }

        return zonedatalist;
    }

    StringMap KFDirAttributeMongo::QueryZoneIp( uint32 zoneid )
    {
        StringMap values;
        auto mongodriver = __DIR_MONGO_DRIVER__;

        auto kfloginid = mongodriver->QueryUInt64( __STRING__( zone ), zoneid, __STRING__( loginzoneid ) );
        if ( kfloginid->_value == 0u )
        {
            return values;
        }

        auto kfipdata = mongodriver->QueryRecord( __STRING__( gate ), kfloginid->_value );
        __DBVALUE_TO_MAP__( kfipdata->_value, values );
        return values;
    }

    StringMap KFDirAttributeMongo::QueryZoneData( uint32 zoneid )
    {
        StringMap zonedata;
        auto mongodriver = __DIR_MONGO_DRIVER__;

        auto kfzonedata = mongodriver->QueryRecord( __STRING__( zone ), zoneid );
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

    bool KFDirAttributeMongo::ZoneBalance( uint32 zoneid, uint32 count )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;

        // 需要mongo实现+操作
        auto kfresult = mongodriver->QueryRecord( __STRING__( zonelist ), zoneid );
        if ( !kfresult->IsOk() )
        {
            return false;
        }

        auto zonecount = kfresult->_value.FindValue( __STRING__( count ) );
        zonecount += count;
        return mongodriver->Insert( __STRING__( zonelist ), zoneid, __STRING__( count ), zonecount );
    }

    bool KFDirAttributeMongo::SetZoneRecommend( uint32 zoneid )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        return mongodriver->Insert( __STRING__( zonerecommend ), 0u, __STRING__( zoneid ), zoneid );
    }

    uint32 KFDirAttributeMongo::GetZoneRecommend()
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __STRING__( zonerecommend ), 0u, __STRING__( zoneid ) );
        return kfresult->_value;
    }

    uint32 KFDirAttributeMongo::BalanceAllocZone()
    {
        auto zoneid = GetZoneRecommend();
        if ( zoneid != _invalid_int )
        {
            return zoneid;
        }

        // 实现按字段排序

        // 找一个人数最少的区
        auto mongodriver = __DIR_MONGO_DRIVER__;

        // 数量升序显示
        KFMongoSelector kfselector;
        kfselector.AddReturn( __STRING__( count ) );
        auto kfresult = mongodriver->QueryListRecord( __STRING__( zonelist ), kfselector );
        for ( auto& dbvalue : kfresult->_value )
        {
            auto zoneid = dbvalue.FindValue( __STRING__( id ) );
            auto kfrecord = mongodriver->QueryRecord( __STRING__( gate ), zoneid );
            if ( kfrecord->IsOk() )
            {
                return zoneid;
            }
        }

        return 1u;
    }

    StringMap KFDirAttributeMongo::AllocPlayerZone( uint32 zoneid )
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

    bool KFDirAttributeMongo::SetWorldUrl( uint64 worldid, const std::string& url )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        return mongodriver->Insert( __STRING__( worldurl ), worldid, __STRING__( url ), url );
    }

    std::string KFDirAttributeMongo::GetWorldUrl( uint64 worldid )
    {
        auto mongodriver = __DIR_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryString( __STRING__( worldurl ), worldid, __STRING__( url ) );
        return kfresult->_value;
    }

    // 更新masterip
    bool KFDirAttributeMongo::UpdateMasterIp( const std::string& appname, uint64 appid, uint32 zoneid, const std::string& ip, uint32 port, uint32 expiretime )
    {
        return true;
    }

    StringMap KFDirAttributeMongo::QueryMasterIp( const std::string& appname, uint32 zoneid )
    {
        StringMap zonedata;
        return zonedata;
    }


    StringListMap KFDirAttributeMongo::QueryMasterList( const std::string& appname, uint32 zoneid )
    {
        StringListMap masterdatalist;
        return masterdatalist;
    }
}