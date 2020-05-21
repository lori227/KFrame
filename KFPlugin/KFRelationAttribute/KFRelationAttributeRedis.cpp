#include "KFRelationAttributeRedis.hpp"
#include "KFBasicAttribute/KFBasicAttributeInterface.h"

namespace KFrame
{
#define __RELATION_REDIS_DRIVER__ _kf_redis->Create( __STRING__( relation ) )

    std::string KFRelationAttributeRedis::FormatRelationKey( const std::string& relationname, uint64 firstid, uint64 secondid, bool bothway )
    {
        auto id1 = firstid;
        auto id2 = secondid;
        if ( bothway )
        {
            id1 = __MIN__( firstid, secondid );
            id2 = __MAX__( firstid, secondid );
        }

        return __FORMAT__( "{}:{}:{}", relationname, id1, id2 );
    }

    void KFRelationAttributeRedis::QueryRelationList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;

        auto queryidlist = redisdriver->QueryList( "smembers {}:{}", listname, playerid );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        for ( auto& strid : queryidlist->_value )
        {
            auto relationid = __TO_UINT64__( strid );

            // 好友的基本信息
            StringMap relationdata;
            _kf_basic_attribute->QueryBasicAttribute( relationid, relationdata );
            if ( relationdata.empty() )
            {
                continue;
            }

            // 好友的关系属性
            auto relationkey = FormatRelationKey( relationname, playerid, relationid, true );
            auto kfquery = redisdriver->QueryMap( "hgetall {}", relationkey );
            relationdata.insert( kfquery->_value.begin(), kfquery->_value.end() );
            relationlist.emplace( relationid, relationdata );
        }
    }

    void KFRelationAttributeRedis::QueryInviteList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;

        auto queryidlist = redisdriver->QueryList( "smembers {}:{}", listname, playerid );
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
            auto kfinvitedata = redisdriver->QueryMap( "hgetall {}", relationkey );
            if ( kfinvitedata->_value.empty() )
            {
                removelist.push_back( strid );
                continue;
            }

            StringMap relationdata;
            _kf_basic_attribute->QueryBasicAttribute( relationid, relationdata );
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
            redisdriver->Update( removelist, "srem {}:{}", listname, playerid );
        }
    }

    bool KFRelationAttributeRedis::RelationExist( const std::string& listname, uint64 playerid, uint64 targetid )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto kfquery = redisdriver->QueryUInt64( "sismember {}:{} {}", listname, playerid, targetid );
        return kfquery->_value != _invalid_int;
    }

    uint32 KFRelationAttributeRedis::RelationCount( const std::string& listname, uint64 playerid )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto kfquery = redisdriver->QueryUInt64( "scard {}:{}", listname, playerid );
        return kfquery->_value;
    }

    void KFRelationAttributeRedis::AddRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto relationkey = FormatRelationKey( relationname, playerid, targetid, bothway );
        redisdriver->Append( "hset {} {} {}", relationkey, __STRING__( time ), KFGlobal::Instance()->_real_time );

        // 添加列表
        redisdriver->Append( "sadd {}:{} {}", listname, playerid, targetid );
        redisdriver->Pipeline();
    }

    void KFRelationAttributeRedis::RemoveRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;

        redisdriver->Append( "srem {}:{} {}", listname, playerid, targetid );

        auto relationkey = FormatRelationKey( relationname, playerid, targetid, bothway );
        redisdriver->Append( "del {}", relationkey );
        redisdriver->Pipeline();
    }

    void KFRelationAttributeRedis::AddInvite( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, const std::string& message, uint64 keeptime )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        redisdriver->Append( "sadd {}:{} {}", listname, playerid, targetid );

        // 加入到申请列表
        auto invitekey = FormatRelationKey( relationname, playerid, targetid, false );
        redisdriver->Append( "hset {} {} {}", invitekey, __STRING__( time ), KFGlobal::Instance()->_real_time );
        if ( !message.empty() )
        {
            redisdriver->Append( "hset {} {} {}", invitekey, __STRING__( message ), message );
        }
        if ( keeptime > 0u )
        {
            redisdriver->Append( "expire {} {}", invitekey, keeptime );
        }
        redisdriver->Pipeline();
    }

    bool KFRelationAttributeRedis::IsRefuse( const std::string& refusename, uint64 playerid )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( refuse ), playerid, refusename );
        return kfresult->_value != _invalid_int;
    }

    void KFRelationAttributeRedis::SetRefuse( const std::string& refusename, uint64 playerid, uint32 refusevalue )
    {
        auto redisdriver = __RELATION_REDIS_DRIVER__;
        redisdriver->Execute( "hset {}:{} {} {}", __STRING__( refuse ), playerid, refusename, refusevalue );
    }
}