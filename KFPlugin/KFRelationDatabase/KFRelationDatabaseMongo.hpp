#ifndef __KF_RELATION_DATABASE_MONGO_H__
#define __KF_RELATION_DATABASE_MONGO_H__

#include "KFRelationDatabaseLogic.hpp"
#include "KFMongo/KFMongoInterface.h"

namespace KFrame
{
    class KFRelationDatabaseMongo : public KFRelationDatabaseLogic
    {
    public:
        KFRelationDatabaseMongo() = default;
        ~KFRelationDatabaseMongo() = default;
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 查询好友列表
        virtual void QueryRelationList( const std::string& list_name, const std::string& relation_name, uint64 player_id, RelationListType& relation_list );

        // 查询申请列表
        virtual void QueryInviteList( const std::string& list_name, const std::string& relation_name, uint64 player_id, RelationListType& relation_list );

        // 查询关系是否存在
        virtual bool RelationExist( const std::string& list_name, uint64 player_id, uint64 target_id );

        // 查询关系数量
        virtual uint32 RelationCount( const std::string& list_name, uint64 player_id );

        // 添加关系
        virtual void AddRelation( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, bool is_both_way );

        // 删除关系
        virtual void RemoveRelation( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, bool is_both_way );

        // 添加邀请数据
        virtual void AddInvite( const std::string& list_name, const std::string& relation_name, uint64 player_id, uint64 target_id, const std::string& message, uint64 keep_time );

        // 是否拒绝
        virtual bool IsRefuse( const std::string& refuse_name, uint64 player_id );

        // 设置拒绝
        virtual void SetRefuse( const std::string& refuse_name, uint64 player_id, uint32 refuse_value );

    protected:
        std::string FormatRelationKey( uint64 first_id, uint64 second_id, bool is_both_way );
    };
}



#endif