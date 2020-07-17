#include "KFNetClientEngine.hpp"
#include "KFNetClientServices.hpp"
#include "KFNetEvent.hpp"

namespace KFrame
{
    KFNetClientEngine::KFNetClientEngine()
    {
        _net_client_services = nullptr;
        _client_connect_function = nullptr;
        _client_disconnect_function = nullptr;
        _client_failed_function = nullptr;
    }

    KFNetClientEngine::~KFNetClientEngine()
    {
        delete _net_client_services;
    }

    void KFNetClientEngine::InitEngine( uint64 nowtime, uint32 queuesize, uint32 messagetype )
    {
        // 网络服务
        _net_client_services = new KFNetClientServices();
        _net_client_services->InitServices( 100, queuesize, messagetype );
        _net_client_services->StartServices( nullptr );
        _net_client_services->_now_time = nowtime;

        // 注册网络时间
        _net_client_services->_net_event->BindDisconnectFunction( this, &KFNetClientEngine::OnClientDisconnect );
        _net_client_services->_net_event->BindShutFunction( this, &KFNetClientEngine::OnClientShutDown );
        _net_client_services->_net_event->BindConnectFunction( this, &KFNetClientEngine::OnClientConnected );
        _net_client_services->_net_event->BindFailedFunction( this, &KFNetClientEngine::OnClientFailed );
    }

    void KFNetClientEngine::ShutEngine()
    {
        for ( auto iter : _kf_clients._objects )
        {
            auto kfclient = iter.second;
            kfclient->CloseClient();
        }

        _wait_clients.clear();
        _net_client_services->ShutServices();
    }

    void KFNetClientEngine::RunEngine( uint64 nowtime )
    {
        // 网络事件
        _net_client_services->_now_time = nowtime;
        _net_client_services->_net_event->RunEvent();

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
            auto kfclient = iter.second;
            kfclient->RunUpdate( _net_function, _max_message_count );
        }
    }

    void KFNetClientEngine::OnClientConnected( const KFNetEventData* eventdata )
    {
        auto kfclient = _kf_clients.Find( eventdata->_id );
        if ( kfclient == nullptr )
        {
            return;
        }

        auto netdata = &kfclient->_net_data;

        __LOG_DEBUG__( "[{}:{}:{}|{}:{}] connect ok", netdata->_name, netdata->_type, netdata->_str_id, netdata->_ip, netdata->_port );

        // 上层回调
        if ( _client_connect_function != nullptr )
        {
            _client_connect_function( netdata );
        }
    }

    void KFNetClientEngine::OnClientDisconnect( const KFNetEventData* eventdata )
    {
        auto kfclient = _kf_clients.Find( eventdata->_id );
        if ( kfclient == nullptr )
        {
            return;
        }

        auto netdata = &kfclient->_net_data;
        __LOG_INFO__( "[{}:{}:{}|{}:{}] disconnect", netdata->_name, netdata->_type, netdata->_str_id, netdata->_ip, netdata->_port );

        // 上层回调
        if ( _client_disconnect_function != nullptr )
        {
            _client_disconnect_function( netdata );
        }
    }

    void KFNetClientEngine::OnClientShutDown( const KFNetEventData* eventdata )
    {
        auto kfclient = _kf_clients.Find( eventdata->_id );
        if ( kfclient != nullptr )
        {
            auto netdata = kfclient->_net_data;
            _kf_clients.Remove( eventdata->_id );

            __LOG_INFO__( "[{}:{}:{}|{}:{}] client shutdown",
                          netdata._name, netdata._type, netdata._str_id, netdata._ip, netdata._port );

            if ( _client_shutdown_function != nullptr )
            {
                _client_shutdown_function( &netdata );
            }
        }
    }

    void KFNetClientEngine::OnClientFailed( const KFNetEventData* eventdata )
    {
        auto kfclient = _kf_clients.Find( eventdata->_id );
        if ( kfclient == nullptr )
        {
            return;
        }

        auto netdata = &kfclient->_net_data;
        __LOG_ERROR__( "[{}:{}:{}|{}:{}] connect failed", netdata->_name, netdata->_type, netdata->_str_id, netdata->_ip, netdata->_port );

        if ( _client_failed_function != nullptr )
        {
            _client_failed_function( netdata );
        }
    }

    bool KFNetClientEngine::StartClient( const KFNetData* netdata )
    {
        auto kfclient = _kf_clients.Find( netdata->_id );
        if ( kfclient != nullptr )
        {
            return false;
        }

        _wait_clients[ netdata->_id ] = *netdata;
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
            auto& netdata = iter.second;
            auto kfclient = _kf_clients.Create( netdata._id );
            kfclient->StartClient( _net_client_services, netdata );
        }

        _wait_clients.clear();
    }

    KFNetClient* KFNetClientEngine::FindClient( uint64 id )
    {
        return _kf_clients.Find( id );
    }

    void KFNetClientEngine::CloseClient( uint64 id, const char* function, uint32 line )
    {
        auto kfclient = _kf_clients.Find( id );
        if ( kfclient == nullptr )
        {
            return;
        }

        kfclient->CloseClient();
    }

    void KFNetClientEngine::SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 delay )
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto netclient = iter.second;
            netclient->SendNetMessage( iter.first, msgid, data, length, delay );
        }
    }

    bool KFNetClientEngine::SendNetMessage( uint64 serverid, uint32 msgid, const char* data, uint32 length, uint32 delay )
    {
        auto netclient = _kf_clients.Find( serverid );
        if ( netclient == nullptr )
        {
            __LOG_ERROR__( "msgid[{}] can't find server[{}]", msgid, KFAppId::ToString( serverid ) );
            return false;
        }

        return netclient->SendNetMessage( msgid, data, length, delay );
    }

    bool KFNetClientEngine::SendNetMessage( uint64 serverid, uint64 recvid, uint32 msgid, const char* data, uint32 length, uint32 delay )
    {
        auto netclient = _kf_clients.Find( serverid );
        if ( netclient == nullptr )
        {
            __LOG_ERROR__( "msgid[{}] can't find server[{}]", msgid, KFAppId::ToString( serverid ) );
            return false;
        }

        return netclient->SendNetMessage( recvid, msgid, data, length, delay );
    }

    void KFNetClientEngine::SendMessageToType( const std::string& servertype, uint32 msgid, const char* data, uint32 length )
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto kfclient = iter.second;
            if ( kfclient->_net_data._type == servertype )
            {
                kfclient->SendNetMessage( iter.first, msgid, data, length );
            }
        }
    }

    void KFNetClientEngine::SendMessageToName( const std::string& servername, uint32 msgid, const char* data, uint32 length )
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto kfclient = iter.second;
            if ( kfclient->_net_data._name == servername )
            {
                kfclient->SendNetMessage( iter.first, msgid, data, length );
            }
        }
    }

    void KFNetClientEngine::SendMessageToServer( const std::string& servername, const std::string& servertype, uint32 msgid, const char* data, uint32 length )
    {
        for ( auto& iter : _kf_clients._objects )
        {
            auto netclient = iter.second;
            if ( netclient->_net_data._name == servername
                    && netclient->_net_data._type == servertype )
            {
                netclient->SendNetMessage( iter.first, msgid, data, length );
            }
        }
    }
}
