#ifndef __KF_PROXY_INTERFACE_H__
#define __KF_PROXY_INTERFACE_H__

#include "KFrame.h"
#include "KFCore/KFAgent.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFProxy
    {
    public:
        virtual ~KFProxy() {}

        // useid
        virtual void SetPlayerId( uint32 playerid ) = 0;
        virtual uint32 GetPlayerId() = 0;

        // gameid
        virtual void SetGameId( uint32 gameid ) = 0;
        virtual uint32 GetGameId() = 0;

        // 发送消息到Client
        virtual bool SendMessageToClient( uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message ) = 0;

        // 发送消息到Game
        virtual bool SendMessageToGame( uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFProxyInterface : public KFModule
    {
    public:
        // 创建代理玩家
        virtual KFProxy* CreateProxy( uint32 playerid ) = 0;

        // 查找代理玩家
        virtual KFProxy* FindProxy( uint32 playerid ) = 0;

        // 删除代理玩家
        virtual bool RemoveProxy( uint32 playerid ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_proxy, KFProxyInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif