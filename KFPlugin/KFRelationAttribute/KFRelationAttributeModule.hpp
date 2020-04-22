#ifndef __KF_RELATION_ATTRIBUTE_MODULE_H__
#define __KF_RELATION_ATTRIBUTE_MODULE_H__

/************************************************************************
//    @Module			:    好友属性逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFProtocol/KFProtocol.h"
#include "KFRelationAttributeInterface.h"
#include "KFRelationAttributeRedis.hpp"
#include "KFRelationAttributeMongo.hpp"

namespace KFrame
{
    class KFRelationAttributeModule : public KFRelationAttributeInterface
    {
    public:
        KFRelationAttributeModule() = default;
        ~KFRelationAttributeModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 查询好友列表
        virtual void QueryRelationList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist );

        // 查询申请列表
        virtual void QueryInviteList( const std::string& listname, const std::string& relationname, uint64 playerid, RelationListType& relationlist );

        // 查询关系是否存在
        virtual bool RelationExist( const std::string& listname, uint64 playerid, uint64 targetid );

        // 查询关系数量
        virtual uint32 RelationCount( const std::string& listname, uint64 playerid );

        // 添加关系
        virtual void AddRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway );

        // 删除关系
        virtual void RemoveRelation( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, bool bothway );

        // 添加邀请数据
        virtual void AddInvite( const std::string& listname, const std::string& relationname, uint64 playerid, uint64 targetid, const std::string& message, uint64 keeptime );

        // 是否拒绝
        virtual bool IsRefuse( const std::string& refusename, uint64 playerid );

        // 设置拒绝
        virtual void SetRefuse( const std::string& refusename, uint64 playerid, uint32 refusevalue );
    protected:
        KFRelationAttributeLogic* _relation_attribute_logic = nullptr;
    };
}



#endif