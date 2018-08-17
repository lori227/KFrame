#ifndef __KF_SERVER_MODULE_H__
#define __KF_SERVER_MODULE_H__

/************************************************************************
//    @Module			:    tcp网络服务器模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-7
************************************************************************/

#include "KFrame.h"
#include "KFTcpServerInterface.h"
#include "KFNetwork/KFNetServerEngine.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFTcpSetting;
    class KFTcpServerModule : public KFTcpServerInterface
    {
    public:
        KFTcpServerModule();
        ~KFTcpServerModule();

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        virtual void ShutDown();

        // 发送消息
        virtual void SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 excludeid = 0 );
        virtual void SendNetMessage( uint32 msgid, google::protobuf::Message* message, uint32 excludeid = 0 );

        virtual bool SendNetMessage( uint32 handleid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendNetMessage( uint32 handleid, uint32 msgid, google::protobuf::Message* message );

        // 给指定对象发送消息
        virtual bool SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, google::protobuf::Message* message );

        // 给指定对象发送消息
        virtual bool SendNetMessage( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendNetMessage( const KFGuid& kfguid, uint32 msgid, google::protobuf::Message* message );

        // 给指定类型发送消息
        virtual void SendMessageToName( const std::string& name, uint32 msgid, google::protobuf::Message* message );
        virtual void SendMessageToType( const std::string& type, uint32 msgid, google::protobuf::Message* message );
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        // 注册到连接器
        virtual bool RegisteNetHandle( uint32 sessionid, uint32 handleid, uint32 objectid );

        // 删除连接器
        virtual bool CloseNetHandle( uint32 handleid, uint32 delaytime, const char* function, uint32 line );

        // 连接数量
        virtual uint32 GetHandleCount();

        // 连接列表
        virtual void GetHandleList( std::list< uint32 >& handlelist );

        // 获得连接ip
        virtual const std::string& GetHandleIp( uint32 handleid );

        // 设置id
        virtual bool SetHandleID( uint32 handleid, uint32 objectid );
        /////////////////////////////////////////////////////////////////////////
    protected:
        // 处理客户端注册
        __KF_MESSAGE_FUNCTION__( HandleRegisterReq );

    private:
        void Run();

        virtual void AddDiscoverFunction( const char* module, KFServerDiscoverFunction& function );
        virtual void RemoveDiscoverFunction( const char* module );

        virtual void AddLostFunction( const char* module, KFServerLostFunction& function );
        virtual void RemoveLostFunction( const char* module );

        virtual void AddTransmitFunction( const char* module, KFTransmitFunction& function );
        virtual void RemoveTransmitFunction( const char* module );

        // 发现客户端回调
        void CallDiscoverFunction( KFNetHandle* tcphandle );

        // 通知有处理器注册
        void BroadcastRegisterToServer( KFNetHandle* tcphandle );

        // 处理断开消息
        void BroadcastLostFromServer( KFNetHandle* nethandle );

        // 通话新客户端所有的客户端的注册信息
        void SendRegisterToServerToHandle( KFNetHandle* tcphandle );

    protected:
        // 消息处理函数
        void HandleNetMessage( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length );

        // 断线处理
        __KF_SERVER_LOST_FUNCTION__( OnServerLostHandle );

        // 丢失客户端回调
        void CallLostFunction( KFNetHandle* tcphandle );

    private:

        // 查找tcpdata
        KFTcpSetting* FindTcpServerSetting();

    private:
        // 网络服务器引擎
        KFNetServerEngine* _kf_server_engine;

        // 发现客户端回调
        KFBind< std::string, const std::string&, KFServerDiscoverFunction > _kf_discover_function;

        // 丢失客户端回调
        KFBind< std::string, const std::string&, KFServerLostFunction > _kf_lost_function;

        // 转发函数
        KFTransmitFunction _kf_transmit_function;
    };
}

#endif