#ifndef __KF_RELATION_CLIENT_INTERFACE_H__
#define __KF_RELATION_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFData;
    class KFEntity;
    class KFRelationClientInterface : public KFModule
    {
    public:
        // 发送消息到关系集群
        virtual bool SendMessageToRelation( uint32 msgid, ::google::protobuf::Message* message ) = 0;

        // 发送消息到关系属性
        virtual bool SendMessageToRelation( KFData* kfrelation, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 添加好友度
        virtual void AddFriendLiness( KFEntity* player, uint64 friendid, uint32 type, uint32 value ) = 0;
        virtual void AddFriendLinessOnce( KFEntity* player, uint64 friendid, uint32 type, uint32 value ) = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // 计算战场成绩, 好友度/ 最近的玩家战绩
        virtual void BalanceBattleRelation( KFEntity* player, uint64 roomid, const KFMsg::PBBattleScore* pbscore ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_relation, KFRelationClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif