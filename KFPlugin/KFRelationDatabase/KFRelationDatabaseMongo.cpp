#include "KFRelationDatabaseMongo.hpp"

namespace KFrame
{
#define __RELATION_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( relation ) )

#define __RELATION_TABLE_NAME__ __STRING__( relation )

    std::string KFRelationDatabaseMongo::FormatRelationKey( uint64 first_id, uint64 second_id, bool is_both_way )
    {
        auto id1 = first_id;
        auto id2 = second_id;
        if ( is_both_way )
        {
            id1 = __MIN__( first_id, second_id );
            id2 = __MAX__( first_id, second_id );
        }

        return __FORMAT__( "{}:{}", id1, id2 );
    }

    void KFRelationDatabaseMongo::QueryRelationList( const std::string& list_name, const std::string& relation_name, uint64 player_id, RelationListType& relation_list )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListUInt64( __RELATION_TABLE_NAME__, player_id, list_name );

        for ( auto relation_id : result->_value )
        {
            // 好友的关系属性
            auto relation_key = FormatRelationKey( player_id, relation_id, true );
            auto query_result = mongo_driver->QueryRecord( relation_name, relation_key );

            StringMap relation_data;
            __DBVALUE_TO_MAP__( query_result->_value, relation_data );
            relation_list.emplace( relation_id, relation_data );
        }
    }

    void KFRelationDatabaseMongo::QueryInviteList( const std::string& list_name, const std::string& relation_name, uint64 player_id, RelationListType& relation_list )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListUInt64( __RELATION_TABLE_NAME__, player_id, list_name );

        UInt64List remove_list;
        for ( auto relation_id : result->_value )
        {
            // 邀请信息
            auto relation_key = FormatRelationKey( player_id, relation_id, false );

            // 获得邀请的数据
            auto invite_data = mongo_driver->QueryRecord( relation_name, relation_key );
            if ( invite_data->_value.IsEmpty() )
            {
                remove_list.push_back( relation_id );
                continue;
            }

            StringMap relation_data;
            __DBVALUE_TO_MAP__( invite_data->_value, relation_data );
            relation_list.emplace( relation_id, relation_data );
        }

        if ( !remove_list.empty() )
        {
            // 删除已经过期的邀请信息
            mongo_driver->Pull( __RELATION_TABLE_NAME__, player_id, list_name, remove_list );
        }
    }

    bool KFRelationDatabaseMongo::RelationExist( const std::string& list_name, uint64 player_id, uint64 target_id )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListUInt64( __RELATION_TABLE_NAME__, player_id, list_name );
        auto iter = std::find( result->_value.begin(), result->_value.end(), target_id );
        return iter != result->_value.end();
    }

    uint32 KFRelationDatabaseMongo::RelationCount( const std::string& list_name, uint64 player_id )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListUInt64( __RELATION_TABLE_NAME__, player_id, list_name );
        return ( uint32 )result->_value.size();
    }

    void KFRelationDatabaseMongo::AddRelation( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, bool is_both_way )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        mongo_driver->Push( __RELATION_TABLE_NAME__, player_id, list_name, target_id );

        auto relation_key = FormatRelationKey( player_id, target_id, is_both_way );
        mongo_driver->Insert( relation_name, relation_key, __STRING__( time ), KFGlobal::Instance()->_real_time );
    }

    void KFRelationDatabaseMongo::RemoveRelation( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, bool is_both_way )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        mongo_driver->Pull( __RELATION_TABLE_NAME__, player_id, list_name, target_id );

        auto relation_key = FormatRelationKey( player_id, target_id, is_both_way );
        mongo_driver->Delete( relation_name, relation_key );
    }

    void KFRelationDatabaseMongo::AddInvite( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, const std::string& message, uint64 keep_time )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        mongo_driver->Push( __RELATION_TABLE_NAME__, player_id, list_name, target_id );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( message ), message );
        db_value.AddValue( __STRING__( time ), KFGlobal::Instance()->_real_time );
        if ( keep_time > 0u )
        {
            mongo_driver->CreateIndex( relation_name, MongoKeyword::_expire );
            db_value.AddValue( MongoKeyword::_expire, keep_time );
        }

        auto relation_key = FormatRelationKey( player_id, target_id, false );
        mongo_driver->Insert( relation_name, relation_key, db_value );
    }

    bool KFRelationDatabaseMongo::IsRefuse( const std::string& refuse_name, uint64 player_id )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        auto result = mongo_driver->QueryUInt64( __RELATION_TABLE_NAME__, player_id, refuse_name );
        return result->_value == _invalid_int;
    }

    void KFRelationDatabaseMongo::SetRefuse( const std::string& refuse_name, uint64 player_id, uint32 refuse_value )
    {
        auto mongo_driver = __RELATION_MONGO_DRIVER__;
        mongo_driver->Insert( __RELATION_TABLE_NAME__, player_id, refuse_name, refuse_value );
    }
}