#ifndef __KF_SERVER_MODULE_H__
#define __KF_SERVER_MODULE_H__

/************************************************************************
//    @Module			:    tcp网络服务器模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-7
************************************************************************/

#include "KFTcpServerInterface.h"
#include "KFTcpServerConfig.hpp"
#include "KFNetwork/KFNetServerEngine.hpp"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFTcpSetting;
    class KFTcpServerModule : public KFTcpServerInterface
    {
    public:
        KFTcpServerModule() = default;
        ~KFTcpServerModule() = default;

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        virtual void ShutDown();

        // 发送消息
        virtual void SendNetMessage( uint32 msgid, const char* data, uint32 length, uint64 excludeid = 0 );
        virtual void SendNetMessage( uint32 msgid, google::protobuf::Message* message, uint64 excludeid = 0 );

        virtual bool SendNetMessage( uint64 handleid, uint32 msgid, const char* data, uint32 length, uint32 delay = 0u );
        virtual bool SendNetMessage( uint64 handleid, uint32 msgid, google::protobuf::Message* message, uint32 delay = 0u );

        // 给指定对象发送消息
        virtual bool SendNetMessage( uint64 handleid, uint64 recvid, uint32 msgid, const char* data, uint32 length, uint32 delay = 0u );
        virtual bool SendNetMessage( uint64 handleid, uint64 recvid, uint32 msgid, google::protobuf::Message* message, uint32 delay = 0u );

        // 给指定类型发送消息
        virtual void SendMessageToName( const std::string& name, uint32 msgid, google::protobuf::Message* message );
        virtual void SendMessageToType( const std::string& type, uint32 msgid, google::protobuf::Message* message );
        virtual void SendMessageToType( const std::string& type, uint32 msgid, const char* data, uint32 length );
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        // 注册到连接器
        virtual bool RegisteNetHandle( uint64 sessionid, uint64 handleid, uint64 objectid );

        // 删除连接器
        virtual bool CloseNetHandle( uint64 handleid, uint32 delaytime, const char* function, uint32 line );

        // 连接数量
        virtual uint32 GetHandleCount();

        // 是否存在连接
        virtual bool HaveHandle( uint64 handleid );

        // 获得连接ip
        virtual const std::string& GetHandleIp( uint64 handleid );

        // 设置id
        virtual bool BindObjectId( uint64 handleid, uint64 objectid );

        // 连接列表
        virtual void GetHandleList( NetDataList& outlist );
        /////////////////////////////////////////////////////////////////////////

    private:

        virtual void AddDiscoverFunction( KFModule* module, KFNetEventFunction& function );
        virtual void RemoveDiscoverFunction( KFModule* module );

        virtual void AddLostFunction( KFModule* module, KFNetEventFunction& function );
        virtual void RemoveLostFunction( KFModule* module );

        virtual void AddTranspondFunction( KFModule* module, KFTranspondFunction& function );
        virtual void RemoveTranspondFunction( KFModule* module );

    protected:
        // 处理客户端注册
        __KF_MESSAGE_FUNCTION__( HandleRegisterReq );

        // 断线处理
        __KF_NET_EVENT_FUNCTION__( OnServerLostHandle );

        // 消息处理函数
        void HandleNetMessage( const Route& route, uint32 msgid, const char* data, uint32 length );

        // 发现客户端回调
        void CallDiscoverFunction( KFNetHandle* tcphandle );

        // 丢失客户端回调
        void CallLostFunction( KFNetHandle* tcphandle );

        // 查找tcpdata
        KFTcpSetting* FindTcpServerSetting();

    private:
        // 网络服务器引擎
        KFNetServerEngine* _server_engine = nullptr;

        // 压缩长度
        uint32 _compress_length = 0u;

        // 是否加密
        bool _is_open_encrypt = false;

        // 发现客户端回调
        KFFunctionMap< KFModule*, KFNetEventFunction > _kf_discover_function;

        // 丢失客户端回调
        KFFunctionMap< KFModule*, KFNetEventFunction > _kf_lost_function;

        // 转发函数
        KFTranspondFunction _kf_transpond_function = nullptr;
    };
}

#endif