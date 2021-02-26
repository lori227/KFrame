#include "KFRelationDatabaseRedis.hpp"
#include "KFBasicDatabase/KFBasicDatabaseInterface.h"

namespace KFrame
{
#define __RELATION_REDIS_DRIVER__ _kf_redis->Create( __STRING__( logic ) )

    std::string KFRelationDatabaseRedis::FormatRelationKey( const std::string& relation_name, uint64 first_id, uint64 second_id, bool is_both_way )
    {
        auto id1 = first_id;
        auto id2 = second_id;
        if ( is_both_way )
        {
            id1 = __MIN__( first_id, second_id );
            id2 = __MAX__( first_id, second_id );
        }

        return __DATABASE_KEY_3__( relation_name, id1, id2 );
    }

    void KFRelationDatabaseRedis::QueryRelationList( const std::string& list_name, const std::string& relation_name, uint64 player_id, RelationListType& relation_list )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;

        auto query_id_list_result = redis_driver->SMembers( __DATABASE_KEY_2__( list_name, player_id ) );
        if ( query_id_list_result->_value.empty() )
        {
            return;
        }

        for ( auto& id : query_id_list_result->_value )
        {
            auto relation_id = __TO_UINT64__( id );

            // 好友的基本信息
            StringMap relation_data;
            _kf_basic_database->QueryBasicAttribute( relation_id, relation_data );
            if ( relation_data.empty() )
            {
                continue;
            }

            // 好友的关系属性
            auto relation_key = FormatRelationKey( relation_name, player_id, relation_id, true );
            auto query_result = redis_driver->HGetAll( relation_key );
            relation_data.insert( query_result->_value.begin(), query_result->_value.end() );
            relation_list.emplace( relation_id, relation_data );
        }
    }

    void KFRelationDatabaseRedis::QueryInviteList( const std::string& list_name, const std::string& relation_name, uint64 player_id, RelationListType& relation_list )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;

        auto query_id_list_result = redis_driver->SMembers( __DATABASE_KEY_2__( list_name, player_id ) );
        if ( query_id_list_result->_value.empty() )
        {
            return;
        }

        StringList remove_list;
        for ( auto& id : query_id_list_result->_value )
        {
            auto relation_id = __TO_UINT64__( id );

            // 邀请信息
            auto relation_key = FormatRelationKey( relation_name, player_id, relation_id, false );
            auto invite_data = redis_driver->HGetAll( relation_key );
            if ( invite_data->_value.empty() )
            {
                remove_list.push_back( id );
                continue;
            }

            StringMap relation_data;
            _kf_basic_database->QueryBasicAttribute( relation_id, relation_data );
            if ( relation_data.empty() )
            {
                continue;
            }

            relation_data.insert( invite_data->_value.begin(), invite_data->_value.end() );
            relation_list.emplace( relation_id, relation_data );
        }

        if ( !remove_list.empty() )
        {
            // 删除已经过期的邀请信息
            redis_driver->SRem( __DATABASE_KEY_2__( list_name, player_id ), remove_list );
        }
    }

    bool KFRelationDatabaseRedis::RelationExist( const std::string& list_name, uint64 player_id, uint64 target_id )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto query_result = redis_driver->SIsMember( __DATABASE_KEY_2__( list_name, player_id ), target_id );
        return query_result->_value != _invalid_int;
    }

    uint32 KFRelationDatabaseRedis::RelationCount( const std::string& list_name, uint64 player_id )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto query_result = redis_driver->SCard( __DATABASE_KEY_2__( list_name, player_id ) );
        return query_result->_value;
    }

    void KFRelationDatabaseRedis::AddRelation( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, bool is_both_way )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto relation_key = FormatRelationKey( relation_name, player_id, target_id, is_both_way );

        redis_driver->WriteMulti();
        redis_driver->SAdd( __DATABASE_KEY_2__( list_name, player_id ), target_id );
        redis_driver->HSet( relation_key, __STRING__( time ), KFGlobal::Instance()->_real_time );
        redis_driver->WriteExec();
    }

    void KFRelationDatabaseRedis::RemoveRelation( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, bool is_both_way )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto relation_key = FormatRelationKey( relation_name, player_id, target_id, is_both_way );

        redis_driver->WriteMulti();
        redis_driver->Del( relation_key );
        redis_driver->SRem( __DATABASE_KEY_2__( list_name, player_id ), target_id );
        redis_driver->WriteExec();
    }

    void KFRelationDatabaseRedis::AddInvite( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, const std::string& message, uint64 keep_time )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto invite_key = FormatRelationKey( relation_name, player_id, target_id, false );

        StringMap values;
        values[ __STRING__( message ) ] = message;
        values[ __STRING__( time ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );

        redis_driver->WriteMulti();
        redis_driver->SAdd( __DATABASE_KEY_2__( list_name, player_id ), target_id );
        redis_driver->HMSet( invite_key, values );
        if ( keep_time > 0u )
        {
            redis_driver->Expire( invite_key, keep_time );
        }
        redis_driver->WriteExec();
    }

    bool KFRelationDatabaseRedis::IsRefuse( const std::string& refuse_name, uint64 player_id )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto result = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( refuse ), player_id ), refuse_name );
        return result->_value != _invalid_int;
    }

    void KFRelationDatabaseRedis::SetRefuse( const std::string& refuse_name, uint64 player_id, uint32 refuse_value )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( refuse ), player_id ), refuse_name, refuse_value );
    }
}