#include "KFRelationDatabaseMongo.hpp"

namespace KFrame
{
#define __RELATION_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( relation ) )

#define __RELATION_TABLE_NAME__ __STRING__( relation )

    std::string KFRelationDatabaseMongo::FormatRelationKey( uint64 firstid, uint64 secondid, bool bothway )
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

    void KFRelationDatabaseMongo::QueryRelationList( const std::string& listname, const std::string& relationname, uint64 player_id, RelationListType& relationlist )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongo_driver->QueryListUInt64( __RELATION_TABLE_NAME__, player_id, listname );

        for ( auto relationid : kfresult->_value )
        {
            // 好友的关系属性
            auto relationkey = FormatRelationKey( player_id, relationid, true );
            auto kfquery = mongo_driver->QueryRecord( relationname, relationkey );

            StringMap relationdata;
            __DBVALUE_TO_MAP__( kfquery->_value, relationdata );
            relationlist.emplace( relationid, relationdata );
        }
    }

    void KFRelationDatabaseMongo::QueryInviteList( const std::string& listname, const std::string& relationname, uint64 player_id, RelationListType& relationlist )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongo_driver->QueryListUInt64( __RELATION_TABLE_NAME__, player_id, listname );

        UInt64List removelist;
        for ( auto relationid : kfresult->_value )
        {
            // 邀请信息
            auto relationkey = FormatRelationKey( player_id, relationid, false );

            // 获得邀请的数据
            auto kfinvitedata = mongo_driver->QueryRecord( relationname, relationkey );
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
            mongo_driver->Pull( __RELATION_TABLE_NAME__, player_id, listname, removelist );
        }
    }

    bool KFRelationDatabaseMongo::RelationExist( const std::string& listname, uint64 player_id, uint64 targetid )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongo_driver->QueryListUInt64( __RELATION_TABLE_NAME__, player_id, listname );
        auto iter = std::find( kfresult->_value.begin(), kfresult->_value.end(), targetid );
        return iter != kfresult->_value.end();
    }

    uint32 KFRelationDatabaseMongo::RelationCount( const std::string& listname, uint64 player_id )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongo_driver->QueryListUInt64( __RELATION_TABLE_NAME__, player_id, listname );
        return ( uint32 )kfresult->_value.size();
    }

    void KFRelationDatabaseMongo::AddRelation( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, bool bothway )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        mongo_driver->Push( __RELATION_TABLE_NAME__, player_id, listname, targetid );

        auto relationkey = FormatRelationKey( player_id, targetid, bothway );
        mongo_driver->Insert( relationname, relationkey, __STRING__( time ), KFGlobal::Instance()->_real_time );
    }

    void KFRelationDatabaseMongo::RemoveRelation( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, bool bothway )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        mongo_driver->Pull( __RELATION_TABLE_NAME__, player_id, listname, targetid );

        auto relationkey = FormatRelationKey( player_id, targetid, bothway );
        mongo_driver->Delete( relationname, relationkey );
    }

    void KFRelationDatabaseMongo::AddInvite( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, const std::string& message, uint64 keeptime )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        mongo_driver->Push( __RELATION_TABLE_NAME__, player_id, listname, targetid );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( message ), message );
        dbvalue.AddValue( __STRING__( time ), KFGlobal::Instance()->_real_time );
        if ( keeptime > 0u )
        {
            mongo_driver->CreateIndex( relationname, MongoKeyword::_expire );
            dbvalue.AddValue( MongoKeyword::_expire, keeptime );
        }

        auto relationkey = FormatRelationKey( player_id, targetid, false );
        mongo_driver->Insert( relationname, relationkey, dbvalue );
    }

    bool KFRelationDatabaseMongo::IsRefuse( const std::string& refusename, uint64 player_id )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto kfresult = mongo_driver->QueryUInt64( __RELATION_TABLE_NAME__, player_id, refusename );
        return kfresult->_value == _invalid_int;
    }

    void KFRelationDatabaseMongo::SetRefuse( const std::string& refusename, uint64 player_id, uint32 refusevalue )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        mongo_driver->Insert( __RELATION_TABLE_NAME__, player_id, refusename, refusevalue );
    }
}