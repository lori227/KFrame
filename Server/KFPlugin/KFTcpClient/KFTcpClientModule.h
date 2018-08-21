#ifndef __KF_CLIENT_MODULE_H__
#define __KF_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    tcp客户端
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-8
************************************************************************/

#include "KFrame.h"
#include "KFTcpClientInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFNetwork/KFNetClientEngine.h"

namespace KFrame
{
    class KFTcpClientModule : public KFTcpClientInterface
    {
    public:
        KFTcpClientModule();
        ~KFTcpClientModule();

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();
        virtual void Run();

        // 关闭
        virtual void ShutDown();
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        // 添加客户端连接
        virtual void StartClient( const std::string& name, const std::string& type, uint32 id, const std::string& ip, uint32 port );

        // 断开连接
        virtual void CloseClient( uint32 serverid, const char* function, uint32 line );
        /////////////////////////////////////////////////////////////////////////

        // 发送消息
        virtual void SendNetMessage( uint32 msgid, google::protobuf::Message* message );
        virtual bool SendNetMessage( uint32 serverid, uint32 msgid, google::protobuf::Message* message );
        virtual bool SendNetMessage( uint32 serverid, uint32 objectid, uint32 msgid, google::protobuf::Message* message );

        virtual void SendNetMessage( uint32 msgid, const char* data, uint32 length );
        virtual bool SendNetMessage( uint32 serverid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendNetMessage( uint32 serverid, uint32 objectid, uint32 msgid, const char* data, uint32 length );

        virtual void SendMessageToName( const std::string& servername, uint32 msgid, google::protobuf::Message* message );
        virtual void SendMessageToName( const std::string& servername, uint32 msgid, const char* data, uint32 length );;

        virtual void SendMessageToType( const std::string& servertype, uint32 msgid, google::protobuf::Message* message );
        virtual void SendMessageToType( const std::string& servertype, uint32 msgid, const char* data, uint32 length );

        virtual void SendMessageToServer( const std::string& servername, const std::string& servertype, uint32 msgid, google::protobuf::Message* message );
        virtual void SendMessageToServer( const std::string& servername, const std::string& servertype, uint32 msgid, const char* data, uint32 length );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        // 客户端连接
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnected );

        // 客户端断开
        __KF_CLIENT_LOST_FUNCTION__( OnClientDisconnect );

    protected:
        // 注册回馈
        __KF_MESSAGE_FUNCTION__( HandleRegisterAck );

    private:

        // 连接回调
        void AddConnectionFunction( const char* name, KFClientConnectionFunction& function );
        void RemoveConnectionFunction( const char* name );
        void CallClientConnectionFunction( uint32 serverid, const std::string& servername, const std::string& servertype );

        // 断线函数
        virtual void AddLostFunction( const char* name, KFClientLostFunction& function );
        void RemoveLostFunction( const char* name );
        void CallClientLostFunction( uint32 serverid, const std::string& servername, const std::string& servertype );

        // 转发函数
        virtual void AddTransmitFunction( const char* name, KFTransmitFunction& function );
        virtual void RemoveTransmitFunction( const char* name );

        ////////////////////////////////////////////////////////////////
        // 处理客户端消息
        void HandleNetMessage( const KFGuid& guid, uint32 msgid, const char* data, uint32 length );

        // 是否连接自己
        bool IsSelfConnection( const std::string& name, const std::string& type, uint32 id );
    public:
        // 客户端引擎
        KFNetClientEngine* _kf_client_engine;

        // 转发函数
        KFTransmitFunction _kf_transmit_function;

        // 注册成功回调函数
        KFBind< std::string, const std::string&, KFClientConnectionFunction > _kf_connection_function;

        // 客户端掉线
        KFBind< std::string, const std::string&, KFClientLostFunction > _kf_lost_function;
    };
}



#endif