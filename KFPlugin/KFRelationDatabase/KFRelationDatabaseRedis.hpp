#ifndef __KF_RELATION_DATABASE_REDIS_H__
#define __KF_RELATION_DATABASE_REDIS_H__

#include "KFRelationDatabaseLogic.hpp"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
    class KFRelationDatabaseRedis : public KFRelationDatabaseLogic
    {
    public:
        KFRelationDatabaseRedis() = default;
        ~KFRelationDatabaseRedis() = default;
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 查询好友列表
        virtual void QueryRelationList( const std::string& listname, const std::string& relationname, uint64 player_id, RelationListType& relationlist );

        // 查询申请列表
        virtual void QueryInviteList( const std::string& listname, const std::string& relationname, uint64 player_id, RelationListType& relationlist );

        // 查询关系是否存在
        virtual bool RelationExist( const std::string& listname, uint64 player_id, uint64 targetid );

        // 查询关系数量
        virtual uint32 RelationCount( const std::string& listname, uint64 player_id );

        // 添加关系
        virtual void AddRelation( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, bool bothway );

        // 删除关系
        virtual void RemoveRelation( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, bool bothway );

        // 添加邀请数据
        virtual void AddInvite( const std::string& listname, const std::string& relationname, uint64 player_id, uint64 targetid, const std::string& message, uint64 keeptime );

        // 是否拒绝
        virtual bool IsRefuse( const std::string& refusename, uint64 player_id );

        // 设置拒绝
        virtual void SetRefuse( const std::string& refusename, uint64 player_id, uint32 refusevalue );

    protected:
        // 格式化关系名
        std::string FormatRelationKey( const std::string& relationname, uint64 firstid, uint64 secondid, bool bothway );
    };
}



#endif