#ifndef __CLIENT_ENGINE_H__
#define __CLIENT_ENGINE_H__

#include "KFNetClient.hpp"
#include "KFNetClientService.hpp"

namespace KFrame
{
    class KFNetClientService;
    class KFNetClientEngine
    {
    public:

        KFNetClientEngine();
        ~KFNetClientEngine();

        // 初始化引擎
        void InitEngine( uint64 now_time, uint32 queue_size, uint32 message_type );

        // 关闭引擎
        void ShutEngine();

        // 执行逻辑
        void RunEngine( uint64 now_time );

        // 开始一个客户端
        bool StartClient( const KFNetData* net_data );

        // 关闭一个客户端
        void CloseClient( uint64 id, const char* function, uint32 line );

        // 查找客户端
        std::shared_ptr<KFNetClient> FindClient( uint64 id );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        // 发送消息
        void SendNetMessage( uint32 msg_id, const char* data, uint32 length, uint32 delay = 0 );
        bool SendNetMessage( uint64 server_id, uint32 msg_id, const char* data, uint32 length, uint32 delay = 0 );
        bool SendNetMessage( uint64 server_id, uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint32 delay = 0 );

        // 指定发送消息
        void SendMessageToName( const std::string& server_name, uint32 msg_id, const char* data, uint32 length );
        void SendMessageToType( const std::string& server_type, uint32 msg_id, const char* data, uint32 length );
        void SendMessageToServer( const std::string& server_name, const std::string& server_type, uint32 msg_id, const char* data, uint32 length );

    public:
        // 绑定消息事件
        template<class T>
        void BindNetFunction( T* object, void ( T::*handle )( const Route& route, uint32 msg_id, const char* data, uint32 length ) )
        {
            net_client_service->_net_function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
        }

        // 绑定网络事件
        template<class T>
        void BindNetEventFunction( uint32 type, T* object, void ( T::*handle )( const KFNetData* ) )
        {
            auto function = _net_event_function_list.Create( type );
            function->SetFunction( object, std::bind( handle, object, std::placeholders::_1 ) );
        }

    protected:
        // 等待添加的客户端
        void RunWaitClient();

        // 处理所有客户端消息
        void HandleClientMessage();

        // 客户端连接成功
        void OnClientConnected( std::shared_ptr<KFNetEventData>& event_data );

        // 客户端断开连接
        void OnClientDisconnect( std::shared_ptr<KFNetEventData>& event_data );

        // 客户端关闭
        void OnClientShutDown( std::shared_ptr<KFNetEventData>& event_data );

        // 连接失败
        void OnClientFailed( std::shared_ptr<KFNetEventData>& event_data );

        // 回调网络事件函数
        void CallNetEventFunction( std::shared_ptr<KFNetEventData>& event_data, const KFNetData* net_data );
    public:
        // 网络服务
        KFNetClientService* net_client_service;

    protected:
        // 等待启动的客户端
        std::unordered_map<uint64, KFNetData> _wait_clients;

        // 客户端列表
        KFHashMap<uint64, KFNetClient> _kf_clients;

    protected:
        // 网络事件回调函数列表
        KFMapFunction<uint32, KFNetEventFunction> _net_event_function_list;
    };
}

#endif
