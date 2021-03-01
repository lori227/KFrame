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
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFNetwork/KFNetServerEngine.hpp"

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
        virtual void SendNetMessage( uint32 msg_id, const char* data, uint32 length, uint64 exclude_id = 0 );
        virtual void SendNetMessage( uint32 msg_id, google::protobuf::Message* message, uint64 exclude_id = 0 );

        virtual bool SendNetMessage( uint64 handle_id, uint32 msg_id, const char* data, uint32 length, uint32 delay = 0u );
        virtual bool SendNetMessage( uint64 handle_id, uint32 msg_id, google::protobuf::Message* message, uint32 delay = 0u );

        // 给指定对象发送消息
        virtual bool SendNetMessage( uint64 handle_id, uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint32 delay = 0u );
        virtual bool SendNetMessage( uint64 handle_id, uint64 recv_id, uint32 msg_id, google::protobuf::Message* message, uint32 delay = 0u );

        // 给指定类型发送消息
        virtual void SendMessageToName( const std::string& name, uint32 msg_id, google::protobuf::Message* message );
        virtual void SendMessageToType( const std::string& type, uint32 msg_id, google::protobuf::Message* message );
        virtual void SendMessageToType( const std::string& type, uint32 msg_id, const char* data, uint32 length );
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        // 注册到连接器
        virtual bool RegisterNetHandle( uint64 session_id, uint64 handle_id, uint64 object_id );

        // 删除连接器
        virtual bool CloseNetHandle( uint64 handle_id, uint32 delay_time, const char* function, uint32 line );

        // 是否存在连接
        virtual bool HaveHandle( uint64 handle_id );

        // 获得连接ip
        virtual const std::string& GetHandleIp( uint64 handle_id );

        // 设置id
        virtual bool BindObjectId( uint64 handle_id, uint64 object_id );

        // 连接列表
        virtual void GetHandleList( NetDataList& out_list );
        /////////////////////////////////////////////////////////////////////////

    private:

        virtual void AddDiscoverFunction( KFModule* module, KFNetEventFunction& function );
        virtual void RemoveDiscoverFunction( KFModule* module );

        virtual void AddLostFunction( KFModule* module, KFNetEventFunction& function );
        virtual void RemoveLostFunction( KFModule* module );

        virtual void AddForwardFunction( KFModule* module, KFForwardFunction& function );
        virtual void RemoveForwardFunction( KFModule* module );

    protected:
        // 处理客户端注册
        __KF_MESSAGE_FUNCTION__( HandleRegisterReq, KFMsg::RegisterToServerReq );

        // 断线处理
        __KF_NET_EVENT_FUNCTION__( OnServerLostHandle );

        // 消息处理函数
        void HandleNetMessage( const Route& route, uint32 msg_id, const char* data, uint32 length );

        // 发现客户端回调
        void CallDiscoverFunction( std::shared_ptr<KFNetHandle> handle );

        // 丢失客户端回调
        void CallLostFunction( std::shared_ptr<KFNetHandle> handle );

        // 查找tcpdata
        std::shared_ptr<KFTcpSetting> FindTcpServerSetting();
        std::shared_ptr<KFTcpSetting> FindTcpSetting( const std::string& app_name, const std::string& app_type );
    private:
        // 网络服务器引擎
        KFNetServerEngine* _server_engine = nullptr;

        // 网路配置
        std::shared_ptr<KFTcpSetting> _tcp_setting = nullptr;

        // 发现客户端回调
        KFMapModuleFunction<KFModule*, KFNetEventFunction> _kf_discover_function;

        // 丢失客户端回调
        KFMapModuleFunction<KFModule*, KFNetEventFunction> _kf_lost_function;

        // 转发函数
        KFModuleFunction<KFForwardFunction> _kf_forward_function;
    };
}

#endif