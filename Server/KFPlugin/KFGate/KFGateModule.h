#ifndef __KF_GATE_MODULE_H__
#define __KF_GATE_MODULE_H__

/************************************************************************
//    @Moudle			:    网关功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFrame.h"
#include "KFConHash/KFConHash.h"
#include "KFGateInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    class KFGateModule : public KFGateInterface
    {
    public:
        KFGateModule();
        ~KFGateModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 是否连接到登录服务器
        virtual bool IsLoginConnected();

        // 发送消息到登录服务器
        virtual bool SendMessageToLogin( uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendMessageToLogin( uint32 accountid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到游戏服务器
        virtual bool SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendMessageToGame( uint32 gameid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendMessageToGame( uint32 gameid, uint32 playerid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到客户端
        virtual void SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendMessageToClient( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message );

        // 添加链接
        virtual bool AddConnection( uint32 connectid, uint32 playerid );

        // 删除链接
        virtual bool RemoveConnection( uint32 playerid, uint32 delaytime, const char* function, uint32 line );

        // 获得ip
        virtual const std::string& GetIp( uint32 connectid );
    protected:
        // 连接成功
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectionLogin );

        // 断开连接
        __KF_CLIENT_LOST_FUNCTION__( OnClientLostLogin );

    private:
        // login服务器列表
        KFConHash _kf_login_conhash;

        // game 服务器列表
        KFConHash _kf_game_conhash;
    };
}

#endif