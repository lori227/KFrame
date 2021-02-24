#include "KFRelationDatabaseRedis.hpp"
#include "KFBasicDatabase/KFBasicDatabaseInterface.h"

namespace KFrame
{
#define __RELATION_REDIS_DRIVER__ _kf_redis->Create( __STRING__( relation ) )

    std::string KFRelationDatabaseRedis::FormatRelationKey( const std::string& relationname, uint64 firstid, uint64 secondid, bool bothway )
    {
        auto id1 = firstid;
        auto id2 = secondid;
        if ( bothway )
        {
            id1 = __MIN__( firstid, secondid );
            id2 = __MAX__( firstid, secondid );
        }

        return __DATABASE_KEY_3__( relationname, id1, id2 );
    }

    void KFRelationDatabaseRedis::QueryRelationList( const std::string& listname, const std::string& relationname, uint64 player_id, RelationListType& relationlist )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;

        auto queryidlist = redis_driver->SMembers( __DATABASE_KEY_2__( listname, player_id ) );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        for ( auto& strid : queryidlist->_value )
        {
            auto relationid = __TO_UINT64__( strid );

            // 好友的基本信息
            StringMap relationdata;
            _kf_basic_database->QueryBasicAttribute( relationid, relationdata );
            if ( relationdata.empty() )
            {
                continue;
            }

            // 好友的关系属性
            auto relationkey = FormatRelationKey( relationname, player_id, relationid, true );
            auto kfquery = redis_driver->HGetAll( relationkey );
            relationdata.insert( kfquery->_value.begin(), kfquery->_value.end() );
            relationlist.emplace( relationid, relationdata );
        }
    }

    void KFRelationDatabaseRedis::QueryInviteList( const std::string& listname, const std::string& relationname, uint64 player_id, RelationListType& relationlist )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;

        auto queryidlist = redis_driver->SMembers( __DATABASE_KEY_2__( listname, player_id ) );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        StringList removelist;
        for ( auto& strid : queryidlist->_value )
        {
            auto relationid = __TO_UINT64__( strid );

            // 邀请信息
            auto relationkey = FormatRelationKey( relationname, player_id, relationid, false );
            auto kfinvitedata = redis_driver->HGetAll( relationkey );
            if ( kfinvitedata->_value.empty() )
            {
                removelist.push_back( strid );
                continue;
            }

            StringMap relationdata;
            _kf_basic_database->QueryBasicAttribute( relationid, relationdata );
            if ( relationdata.empty() )
            {
                continue;
            }

            relationdata.insert( kfinvitedata->_value.begin(), kfinvitedata->_value.end() );
            relationlist.emplace( relationid, relationdata );
        }

        if ( !removelist.empty() )
        {
            // 删除已经过期的邀请信息
            redis_driver->SRem( __DATABASE_KEY_2__( listname, player_id ), removelist );
        }
    }

    bool KFRelationDatabaseRedis::RelationExist( const std::string& listname, uint64 player_id, uint64 targetid )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto kfquery = redis_driver->SIsMember( __DATABASE_KEY_2__( listname, player_id ), targetid );
        return kfquery->_value != _invalid_int;
    }

    uint32 KFRelationDatabaseRedis::RelationCount( const std::string& listname, uint64 player_id )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto kfquery = redis_driver->SCard( __DATABASE_KEY_2__( listname, player_id ) );
        return kfquery->_value;
    }

    void KFRelationDatabaseRedis::AddRelation( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, bool bothway )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto relationkey = FormatRelationKey( relationname, player_id, targetid, bothway );

        redis_driver->WriteMulti();
        redis_driver->SAdd( __DATABASE_KEY_2__( listname, player_id ), targetid );
        redis_driver->HSet( relationkey, __STRING__( time ), KFGlobal::Instance()->_real_time );
        redis_driver->WriteExec();
    }

    void KFRelationDatabaseRedis::RemoveRelation( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, bool bothway )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto relationkey = FormatRelationKey( relationname, player_id, targetid, bothway );

        redis_driver->WriteMulti();
        redis_driver->Del( relationkey );
        redis_driver->SRem( __DATABASE_KEY_2__( listname, player_id ), targetid );
        redis_driver->WriteExec();
    }

    void KFRelationDatabaseRedis::AddInvite( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, const std::string& message, uint64 keeptime )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto invitekey = FormatRelationKey( relationname, player_id, targetid, false );

        StringMap values;
        values[ __STRING__( message ) ] = message;
        values[ __STRING__( time ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );

        redis_driver->WriteMulti();
        redis_driver->SAdd( __DATABASE_KEY_2__( listname, player_id ), targetid );
        redis_driver->HMSet( invitekey, values );
        if ( keeptime > 0u )
        {
            redis_driver->Expire( invitekey, keeptime );
        }
        redis_driver->WriteExec();
    }

    bool KFRelationDatabaseRedis::IsRefuse( const std::string& refusename, uint64 player_id )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        auto kfresult = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( refuse ), player_id ), refusename );
        return kfresult->_value != _invalid_int;
    }

    void KFRelationDatabaseRedis::SetRefuse( const std::string& refusename, uint64 player_id, uint32 refusevalue )
    {
        auto redis_driver = __RELATION_REDIS_DRIVER__;
        redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( refuse ), player_id ), refusename, refusevalue );
    }
}