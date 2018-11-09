#ifndef __KF_GROUP_CLIENT_INTERFACE_H__
#define __KF_GROUP_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFEntity;
    class KFGroupClientInterface : public KFModule
    {
    public:
        // 发送消息到Group
        virtual bool SendToGroup( uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToGroup( uint64 groupid, uint32 msgid, ::google::protobuf::Message* message ) = 0;

        // 判断是否是队员
        virtual bool IsGroupMember( KFEntity* player, uint32 playerid ) = 0;

        // 判断是否是队长
        virtual bool IsGroupCaptain( KFEntity* player, uint32 playerid ) = 0;

        // 删除一个队员
        virtual void RemoveGroupMember( uint64 groupid, uint32 playerid ) = 0;

        // 队员数量(没有队伍为1)
        virtual uint32 GroupMemberCount( KFEntity* player ) = 0;

        // 是否在队伍中
        virtual bool IsInGroup( KFEntity* player ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_group, KFGroupClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif