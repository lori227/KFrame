#include "KFNetClientEngine.hpp"
#include "KFNetEvent.hpp"

namespace KFrame
{
    KFNetClientEngine::KFNetClientEngine()
    {
        net_client_service = nullptr;
        _net_event_function_list.Clear();
    }

    KFNetClientEngine::~KFNetClientEngine()
    {
        __DELETE_OBJECT__( net_client_service );
    }

    void KFNetClientEngine::InitEngine( uint64 now_time, uint32 queue_size, uint32 message_type )
    {
        // 网络服务
        net_client_service = __NEW_OBJECT__( KFNetClientService );
        net_client_service->InitService( 100, queue_size, message_type );
        net_client_service->StartService( nullptr );
        net_client_service->_now_time = now_time;

        // 注册网络事件
        net_client_service->_net_event->BindEventFunction( KFNetDefine::ShutEvent, this, &KFNetClientEngine::OnClientShutDown );
        net_client_service->_net_event->BindEventFunction( KFNetDefine::FailedEvent, this, &KFNetClientEngine::OnClientFailed );
        net_client_service->_net_event->BindEventFunction( KFNetDefine::ConnectEvent, this, &KFNetClientEngine::OnClientConnected );
        net_client_service->_net_event->BindEventFunction( KFNetDefine::DisconnectEvent, this, &KFNetClientEngine::OnClientDisconnect );
    }

    void KFNetClientEngine::ShutEngine()
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto client = iter.second;
            client->CloseClient();
        }

        _wait_clients.clear();
        net_client_service->CloseService();
    }

    void KFNetClientEngine::RunEngine( uint64 now_time )
    {
        // 网络事件
        net_client_service->_now_time = now_time;
        net_client_service->_net_event->RunEvent();

        // 添加客户端
        RunWaitClient();

        // 处理所有客户端消息
        HandleClientMessage();
    }

    void KFNetClientEngine::HandleClientMessage()
    {
        // 每次取500个消息, 防止占用过多的cpu
        static const uint32 _max_message_count = 500;

        for ( auto iter : _kf_clients._objects )
        {
            auto client = iter.second;
            client->RunUpdate( _max_message_count );
        }
    }

    void KFNetClientEngine::CallNetEventFunction( std::shared_ptr<KFNetEventData>& event_data, const KFNetData* net_data )
    {
        auto function = _net_event_function_list.Find( event_data->_type );
        if ( function != nullptr )
        {
            function->Call( net_data );
        }
    }

    void KFNetClientEngine::OnClientConnected( std::shared_ptr<KFNetEventData>& event_data )
    {
        auto client = _kf_clients.Find( event_data->_id );
        if ( client == nullptr )
        {
            return;
        }

        auto net_data = &client->_net_data;

        __LOG_INFO__( "[{}:{}:{}|{}:{}] connect ok", net_data->_name, net_data->_type, net_data->_str_id, net_data->_ip, net_data->_port );

        // 上层回调
        CallNetEventFunction( event_data, net_data );
    }

    void KFNetClientEngine::OnClientDisconnect( std::shared_ptr<KFNetEventData>& event_data )
    {
        auto client = _kf_clients.Find( event_data->_id );
        if ( client == nullptr )
        {
            return;
        }

        auto net_data = &client->_net_data;
        __LOG_INFO__( "[{}:{}:{}|{}:{}] disconnect", net_data->_name, net_data->_type, net_data->_str_id, net_data->_ip, net_data->_port );

        // 上层回调
        CallNetEventFunction( event_data, net_data );
    }

    void KFNetClientEngine::OnClientShutDown( std::shared_ptr<KFNetEventData>& event_data )
    {
        auto client = _kf_clients.Find( event_data->_id );
        if ( client != nullptr )
        {
            auto net_data = client->_net_data;
            _kf_clients.Remove( event_data->_id );

            __LOG_INFO__( "[{}:{}:{}|{}:{}] client shutdown",
                          net_data._name, net_data._type, net_data._str_id, net_data._ip, net_data._port );

            // 上层回调
            CallNetEventFunction( event_data, &net_data );
        }
    }

    void KFNetClientEngine::OnClientFailed( std::shared_ptr<KFNetEventData>& event_data )
    {
        auto client = _kf_clients.Find( event_data->_id );
        if ( client == nullptr )
        {
            return;
        }

        auto net_data = &client->_net_data;
        __LOG_ERROR__( "[{}:{}:{}|{}:{}] connect failed", net_data->_name, net_data->_type, net_data->_str_id, net_data->_ip, net_data->_port );

        // 上层回调
        CallNetEventFunction( event_data, net_data );
    }

    bool KFNetClientEngine::StartClient( const KFNetData* net_data )
    {
        auto client = _kf_clients.Find( net_data->_id );
        if ( client != nullptr )
        {
            return false;
        }

        _wait_clients[ net_data->_id ] = *net_data;
        return true;
    }

    void KFNetClientEngine::RunWaitClient()
    {
        if ( _wait_clients.empty() )
        {
            return;
        }

        for ( auto& iter : _wait_clients )
        {
            auto& net_data = iter.second;
            auto client = _kf_clients.Create( net_data._id );
            client->StartClient( net_client_service, net_data );
        }

        _wait_clients.clear();
    }

    std::shared_ptr<KFNetClient> KFNetClientEngine::FindClient( uint64 id )
    {
        return _kf_clients.Find( id );
    }

    void KFNetClientEngine::CloseClient( uint64 id, const char* function, uint32 line )
    {
        auto client = _kf_clients.Find( id );
        if ( client == nullptr )
        {
            return;
        }

        client->CloseClient();
    }

    void KFNetClientEngine::SendNetMessage( uint32 msg_id, const char* data, uint32 length, uint32 delay )
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto client = iter.second;
            client->SendNetMessage( iter.first, msg_id, data, length, delay );
        }
    }

    bool KFNetClientEngine::SendNetMessage( uint64 server_id, uint32 msg_id, const char* data, uint32 length, uint32 delay )
    {
        auto client = _kf_clients.Find( server_id );
        if ( client == nullptr )
        {
            __LOG_ERROR__( "msg_id[{}] can't find server[{}]", msg_id, KFAppId::ToString( server_id ) );
            return false;
        }

        return client->SendNetMessage( msg_id, data, length, delay );
    }

    bool KFNetClientEngine::SendNetMessage( uint64 server_id, uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint32 delay )
    {
        auto client = _kf_clients.Find( server_id );
        if ( client == nullptr )
        {
            __LOG_ERROR__( "msg_id[{}] can't find server[{}]", msg_id, KFAppId::ToString( server_id ) );
            return false;
        }

        return client->SendNetMessage( recv_id, msg_id, data, length, delay );
    }

    void KFNetClientEngine::SendMessageToType( const std::string& server_type, uint32 msg_id, const char* data, uint32 length )
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto client = iter.second;
            if ( client->_net_data._type == server_type )
            {
                client->SendNetMessage( iter.first, msg_id, data, length );
            }
        }
    }

    void KFNetClientEngine::SendMessageToName( const std::string& server_name, uint32 msg_id, const char* data, uint32 length )
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto client = iter.second;
            if ( client->_net_data._name == server_name )
            {
                client->SendNetMessage( iter.first, msg_id, data, length );
            }
        }
    }

    void KFNetClientEngine::SendMessageToServer( const std::string& server_name, const std::string& server_type, uint32 msg_id, const char* data, uint32 length )
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto client = iter.second;
            if ( client->_net_data._name == server_name
                    && client->_net_data._type == server_type )
            {
                client->SendNetMessage( iter.first, msg_id, data, length );
            }
        }
    }
}
