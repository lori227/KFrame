#include "KFRelationAttributeMongo.hpp"

namespace KFrame
{
#define __RELATION_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( relation ) )

#define __RELATION_TABLE_NAME__ __STRING__( relation )

    std::string KFRelationAttributeMongo::FormatRelationKey( uint64 firstid, uint64 secondid, bool bothway )
    {
        auto id1 = firstid;
        auto id2 = secondid;
        if ( bothway )
        {
            id1 = __MIN__( firstid, secondid );
            id2 = __MAX__( firstid, secondid );
        }

        return __FORMAT__( "{}:{}", id1, id2 );
    }

    void KFRelationAttributeMongo::QueryRelationList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListUInt64( __RELATION_TABLE_NAME__, playerid, listname );

        for ( auto relationid : kfresult->_value )
        {
            // 好友的关系属性
            auto relationkey = FormatRelationKey( playerid, relationid, true );
            auto kfquery = mongodriver->QueryRecord( relationname, relationkey );

            StringMap relationdata;
            __DBVALUE_TO_MAP__( kfquery->_value, relationdata );
            relationlist.emplace( relationid, relationdata );
        }
    }

    void KFRelationAttributeMongo::QueryInviteList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListUInt64( __RELATION_TABLE_NAME__, playerid, listname );

        UInt64List removelist;
        for ( auto relationid : kfresult->_value )
        {
            // 邀请信息
            auto relationkey = FormatRelationKey( playerid, relationid, false );

            // 获得邀请的数据
            auto kfinvitedata = mongodriver->QueryRecord( relationname, relationkey );
            if ( kfinvitedata->_value.IsEmpty() )
            {
                removelist.push_back( relationid );
                continue;
            }

            StringMap relationdata;
            __DBVALUE_TO_MAP__( kfinvitedata->_value, relationdata );
            relationlist.emplace( relationid, relationdata );
        }

        if ( !removelist.empty() )
        {
            // 删除已经过期的邀请信息
            mongodriver->Pull( __RELATION_TABLE_NAME__, playerid, listname, removelist );
        }
    }

    bool KFRelationAttributeMongo::RelationExist( const std::string& listname, uint64 playerid, uint64 targetid )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListUInt64( __RELATION_TABLE_NAME__, playerid, listname );
        auto iter = std::find( kfresult->_value.begin(), kfresult->_value.end(), targetid );
        return iter != kfresult->_value.end();
    }

    uint32 KFRelationAttributeMongo::RelationCount( const std::string& listname, uint64 playerid )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListUInt64( __RELATION_TABLE_NAME__, playerid, listname );
        return ( uint32 )kfresult->_value.size();
    }

    void KFRelationAttributeMongo::AddRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        mongodriver->Push( __RELATION_TABLE_NAME__, playerid, listname, targetid );

        auto relationkey = FormatRelationKey( playerid, targetid, bothway );
        mongodriver->Insert( relationname, relationkey, __STRING__( time ), KFGlobal::Instance()->_real_time );
    }

    void KFRelationAttributeMongo::RemoveRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        mongodriver->Pull( __RELATION_TABLE_NAME__, playerid, listname, targetid );

        auto relationkey = FormatRelationKey( playerid, targetid, bothway );
        mongodriver->Delete( relationname, relationkey );
    }

    void KFRelationAttributeMongo::AddInvite( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, const std::string& message, uint64 keeptime )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        mongodriver->Push( __RELATION_TABLE_NAME__, playerid, listname, targetid );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( message ), message );
        dbvalue.AddValue( __STRING__( time ), KFGlobal::Instance()->_real_time );
        if ( keeptime > 0u )
        {
            mongodriver->CreateIndex( relationname, MongoKeyword::_expire );
            dbvalue.AddValue( MongoKeyword::_expire, keeptime );
        }

        auto relationkey = FormatRelationKey( playerid, targetid, false );
        mongodriver->Insert( relationname, relationkey, dbvalue );
    }

    bool KFRelationAttributeMongo::IsRefuse( const std::string& refusename, uint64 playerid )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __RELATION_TABLE_NAME__, playerid, refusename );
        return kfresult->_value == _invalid_int;
    }

    void KFRelationAttributeMongo::SetRefuse( const std::string& refusename, uint64 playerid, uint32 refusevalue )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        mongodriver->Insert( __RELATION_TABLE_NAME__, playerid, refusename, refusevalue );
    }
}