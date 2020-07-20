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

    void KFRelationDatabaseRedis::QueryRelationList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;

        auto queryidlist = redisdriver->SMembers( __DATABASE_KEY_2__( listname, playerid ) );
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
            auto relationkey = FormatRelationKey( relationname, playerid, relationid, true );
            auto kfquery = redisdriver->HGetAll( relationkey );
            relationdata.insert( kfquery->_value.begin(), kfquery->_value.end() );
            relationlist.emplace( relationid, relationdata );
        }
    }

    void KFRelationDatabaseRedis::QueryInviteList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;

        auto queryidlist = redisdriver->SMembers( __DATABASE_KEY_2__( listname, playerid ) );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        StringList removelist;
        for ( auto& strid : queryidlist->_value )
        {
            auto relationid = __TO_UINT64__( strid );

            // 邀请信息
            auto relationkey = FormatRelationKey( relationname, playerid, relationid, false );
            auto kfinvitedata = redisdriver->HGetAll( relationkey );
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
            redisdriver->SRem( __DATABASE_KEY_2__( listname, playerid ), removelist );
        }
    }

    bool KFRelationDatabaseRedis::RelationExist( const std::string& listname, uint64 playerid, uint64 targetid )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto kfquery = redisdriver->SIsMember( __DATABASE_KEY_2__( listname, playerid ), targetid );
        return kfquery->_value != _invalid_int;
    }

    uint32 KFRelationDatabaseRedis::RelationCount( const std::string& listname, uint64 playerid )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto kfquery = redisdriver->SCard( __DATABASE_KEY_2__( listname, playerid ) );
        return kfquery->_value;
    }

    void KFRelationDatabaseRedis::AddRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto relationkey = FormatRelationKey( relationname, playerid, targetid, bothway );

        redisdriver->WriteMulti();
        redisdriver->SAdd( __DATABASE_KEY_2__( listname, playerid ), targetid );
        redisdriver->HSet( relationkey, __STRING__( time ), KFGlobal::Instance()->_real_time );
        redisdriver->WriteExec();
    }

    void KFRelationDatabaseRedis::RemoveRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto relationkey = FormatRelationKey( relationname, playerid, targetid, bothway );

        redisdriver->WriteMulti();
        redisdriver->Del( relationkey );
        redisdriver->SRem( __DATABASE_KEY_2__( listname, playerid ), targetid );
        redisdriver->WriteExec();
    }

    void KFRelationDatabaseRedis::AddInvite( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, const std::string& message, uint64 keeptime )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto invitekey = FormatRelationKey( relationname, playerid, targetid, false );

        StringMap values;
        values[ __STRING__( message ) ] = message;
        values[ __STRING__( time ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );

        redisdriver->WriteMulti();
        redisdriver->SAdd( __DATABASE_KEY_2__( listname, playerid ), targetid );
        redisdriver->HMSet( invitekey, values );
        if ( keeptime > 0u )
        {
            redisdriver->Expire( invitekey, keeptime );
        }
        redisdriver->WriteExec();
    }

    bool KFRelationDatabaseRedis::IsRefuse( const std::string& refusename, uint64 playerid )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( refuse ), playerid ), refusename );
        return kfresult->_value != _invalid_int;
    }

    void KFRelationDatabaseRedis::SetRefuse( const std::string& refusename, uint64 playerid, uint32 refusevalue )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        redisdriver->HSet( __DATABASE_KEY_2__( __STRING__( refuse ), playerid ), refusename, refusevalue );
    }
}