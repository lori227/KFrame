#ifndef __KF_WORLD_INTERFACE_H__
#define __KF_WORLD_INTERFACE_H__

#include "KFOnline.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFWorldInterface : public KFModule
    {
    public:
        // 创建在线玩家
        virtual KFOnline* CreateOnline( uint32 playerid ) = 0;

        // 查找在线玩家
        virtual KFOnline* FindOnline( uint32 playerid ) = 0;

        // 在线玩家的总人数
        virtual uint32 GetOnlineCount() = 0;

        // 删除在线玩家
        virtual bool RemoveOnline( uint32 playerid ) = 0;

        // 踢掉在线玩家
        virtual bool KickOnline( uint32 playerid, const char* function, uint32 line ) = 0;

        // 发送消息到玩家
        virtual bool SendMessageToOnline( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message ) = 0;

        // 发送消息到游戏服务器
        virtual bool SendMessageToGame( uint32 gameid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_world, KFWorldInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif