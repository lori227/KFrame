#include "KFRelationAttributeMongo.hpp"
#include "KFBasicAttribute/KFBasicAttributeInterface.h"

namespace KFrame
{
#define __RELATION_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( relation ) )

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
        auto kfresult = mongodriver->QueryListUInt64( listname, playerid, __STRING__( player ) );

        for ( auto relationid : kfresult->_value )
        {
            // 好友的基本信息
            StringMap relationdata;
            _kf_basic_attribute->QueryBasicAttribute( relationid, relationdata );
            if ( relationdata.empty() )
            {
                continue;
            }

            // 好友的关系属性
            auto relationkey = FormatRelationKey( playerid, relationid, true );
            auto kfquery = mongodriver->QueryRecord( relationname, relationkey );
            __DBVALUE_TO_MAP__( kfquery->_value, relationdata );
            relationlist.emplace( relationid, relationdata );
        }
    }

    void KFRelationAttributeMongo::QueryInviteList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListUInt64( listname, playerid, __STRING__( player ) );

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
            _kf_basic_attribute->QueryBasicAttribute( relationid, relationdata );
            if ( relationdata.empty() )
            {
                continue;
            }

            __DBVALUE_TO_MAP__( kfinvitedata->_value, relationdata );
            relationlist.emplace( relationid, relationdata );
        }

        if ( !removelist.empty() )
        {
            // 删除已经过期的邀请信息
            mongodriver->Pull( listname, playerid, __STRING__( player ), removelist );
        }
    }

    bool KFRelationAttributeMongo::RelationExist( const std::string& listname, uint64 playerid, uint64 targetid )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListUInt64( listname, playerid, __STRING__( player ) );
        auto iter = std::find( kfresult->_value.begin(), kfresult->_value.end(), targetid );
        return iter != kfresult->_value.end();
    }

    uint32 KFRelationAttributeMongo::RelationCount( const std::string& listname, uint64 playerid )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListUInt64( listname, playerid, __STRING__( player ) );
        return ( uint32 )kfresult->_value.size();
    }

    void KFRelationAttributeMongo::AddRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        mongodriver->Push( listname, playerid, __STRING__( player ), targetid );

        auto relationkey = FormatRelationKey( playerid, targetid, bothway );
        mongodriver->Insert( relationname, relationkey, __STRING__( time ), KFGlobal::Instance()->_real_time );
    }

    void KFRelationAttributeMongo::RemoveRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        mongodriver->Pull( listname, playerid, __STRING__( player ), targetid );

        auto relationkey = FormatRelationKey( playerid, targetid, bothway );
        mongodriver->Delete( relationname, relationkey );
    }

    void KFRelationAttributeMongo::AddInvite( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, const std::string& message, uint64 keeptime )
    {
        auto mongodriver = __RELATION_MONGO_DRIVER__;
        mongodriver->Push( listname, playerid, __STRING__( player ), targetid );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( message ), message );
        dbvalue.AddValue( __STRING__( time ), KFGlobal::Instance()->_real_time );
        if ( keeptime > 0u )
        {
            CreateRelationIndex( mongodriver, relationname );
            dbvalue.AddValue( MongoKeyword::_expire, keeptime );
        }

        auto relationkey = FormatRelationKey( playerid, targetid, false );
        mongodriver->Insert( relationname, relationkey, dbvalue );
    }

    void KFRelationAttributeMongo::CreateRelationIndex( KFMongoDriver* mongodriver, const std::string& relationname )
    {
        auto create = _relation_index_create[ relationname ];
        if ( create == 1u )
        {
            return;
        }

        auto ok = mongodriver->CreateIndex( relationname, MongoKeyword::_expire );
        if ( ok )
        {
            _relation_index_create[ relationname ] = 1u;
        }
    }
}