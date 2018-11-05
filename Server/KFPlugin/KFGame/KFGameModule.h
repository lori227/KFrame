#ifndef __KF_GAME_MODULE_H__
#define __KF_GAME_MODULE_H__

/************************************************************************
//    @Module			:    游戏功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFrame.h"
#include "KFGameInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    class KFGameModule : public KFGameInterface
    {
    public:
        KFGameModule();
        ~KFGameModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 发送消息到世界服务器
        virtual bool SendToWorld( uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到Gate服务器
        virtual void SendToGate( uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToGate( uint32 gateid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到客户端
        virtual bool SendToClient( uint32 gateid, uint32 playerid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToClient( uint32 gateid, uint32 playerid, uint32 msgid, const char* data, uint32 length );

        // 广播消息到客户端
        virtual bool BroadcastToGate( uint32 msgid, ::google::protobuf::Message* message );
        virtual bool BroadcastToWorld( uint32 msgid, ::google::protobuf::Message* message );

        // 消息转发
        virtual bool TransmitToPlayer( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message );

        // 转发服务器
        virtual bool TransmitToServer( uint32 msgid, ::google::protobuf::Message* message );

    protected:
        // 处理消息广播
        __KF_MESSAGE_FUNCTION__( HandleBroadcastMessageReq );

    protected:
        // 连接成功
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectionServer );

        // 断开连接
        __KF_CLIENT_LOST_FUNCTION__( OnClientLostServer );

    private:
        // 世界服务器id
        uint32 _world_server_id;
    };
}

#endif