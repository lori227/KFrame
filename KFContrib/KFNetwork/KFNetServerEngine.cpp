#include "KFNetServerEngine.hpp"

namespace KFrame
{
    KFNetServerEngine::KFNetServerEngine()
    {
        _net_server_service = nullptr;
    }

    KFNetServerEngine::~KFNetServerEngine()
    {
        __DELETE_OBJECT__( _net_server_service );
    }

    ////////////////////////////////////////////////////
    void KFNetServerEngine::InitEngine( uint32 max_queue_size, uint32 handle_count, uint32 message_type,
                                        uint32 compress_type, uint32 compress_level, uint32 compress_length,
                                        const std::string& encrypt_key, bool open_encrypt )
    {
        _handle_message_count = handle_count;
        _net_server_service = __NEW_OBJECT__( KFNetServerService );
        _net_server_service->InitService( 10000, max_queue_size, message_type );
        _net_server_service->InitCompress( compress_type, compress_level, compress_length );
        _net_server_service->InitEncrypt( encrypt_key, open_encrypt );
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        _net_server_service->_net_event->BindEventFunction( KFNetDefine::ShutEvent, this, &KFNetServerEngine::OnServerShutDown );
        _net_server_service->_net_event->BindEventFunction( KFNetDefine::ShutEvent, this, &KFNetServerEngine::OnServerDisconnect );
        _net_server_service->_net_event->BindEventFunction( KFNetDefine::ConnectEvent, this, &KFNetServerEngine::OnServerConnected );

    }

    int32 KFNetServerEngine::StartEngine( const std::string& ip, uint32 port )
    {
        KFNetData net_data;
        net_data._ip = ip;
        net_data._port = port;
        return _net_server_service->StartService( &net_data );
    }

    void KFNetServerEngine::ShutEngine()
    {
        {
            auto handle = _trustee_handle_list.First();
            while ( handle != nullptr )
            {
                handle->CloseHandle();
                handle = _trustee_handle_list.Next();
            }
        }

        {
            auto handle = _work_handle_list.First();
            while ( handle != nullptr )
            {
                handle->CloseHandle();
                handle = _work_handle_list.Next();
            }
        }

        _bind_timeout_list.clear();
        _net_server_service->CloseService();
    }

    void KFNetServerEngine::RunEngine( uint64 now_time )
    {
        _net_server_service->_now_time = now_time;
        _net_server_service->_net_event->RunEvent();

        // 判断托管超时
        RunCheckBindTimeout();

        // 需要关闭的连接
        RunCloseHandle();

        // 托管连接消息
        RunTrusteeMessage( now_time );
        RunRemoveTrusteeHandle();

        // 注册连接消息
        RunHandleMessage( now_time );
    }

    uint32 KFNetServerEngine::GetHandleCount()
    {
        return _work_handle_list.Size();
    }

    void KFNetServerEngine::GetHandleList( NetDataList& out_list )
    {
        out_list.clear();

        auto handle = _work_handle_list.First();
        while ( handle != nullptr )
        {
            out_list.push_back( &handle->_net_data );
            handle = _work_handle_list.Next();
        }
    }

    std::shared_ptr<KFNetHandle> KFNetServerEngine::FindNetHandle( uint64 handle_id )
    {
        return _work_handle_list.Find( handle_id );
    }

    const std::string& KFNetServerEngine::GetHandleIp( uint64 handle_id )
    {
        auto handle = FindNetHandle( handle_id );
        if ( handle == nullptr )
        {
            return _invalid_string;
        }

        return handle->_remote_ip;
    }

    void KFNetServerEngine::OnServerConnected( std::shared_ptr<KFNetEventData>& event_data )
    {
        // 加入托管列表
        auto handle = static_pointer_cast<KFNetHandle>( event_data->_data );
        _trustee_handle_list.Insert( event_data->_id, handle );

        // 1分钟没有验证绑定, 自动断开
        _bind_timeout_list[ event_data->_id ] = _net_server_service->_now_time + 60000;
    }

    void KFNetServerEngine::OnServerShutDown( std::shared_ptr<KFNetEventData>& event_data )
    {
        auto handle = _trustee_handle_list.Remove( event_data->_id );
        if ( handle == nullptr )
        {
            handle = _work_handle_list.Remove( event_data->_id );
            if ( handle == nullptr )
            {
                return __LOG_ERROR__( "handle[{}:{}] shutdown failed", event_data->_id, KFAppId::ToString( event_data->_id ) );
            }
        }

        __LOG_INFO__( "handle[{}:{}] shutdown ok", event_data->_id, KFAppId::ToString( event_data->_id ) );
    }

    void KFNetServerEngine::OnServerDisconnect( std::shared_ptr<KFNetEventData>& event_data )
    {
        // 断开连接
        auto handle = _work_handle_list.Find( event_data->_id );
        if ( handle != nullptr )
        {
            auto function = _event_function_list.Find( KFNetDefine::DisconnectEvent );
            if ( function != nullptr )
            {
                function->Call( &handle->_net_data );
            }
        }
        else
        {
            handle = _trustee_handle_list.Find( event_data->_id );
            if ( handle == nullptr )
            {
                return __LOG_ERROR__( "can't find handle[{}]", event_data->_id );
            }
        }

        handle->CloseHandle();
    }

    bool KFNetServerEngine::CloseHandle( uint64 id, uint32 delay_time, const char* function, uint32 line )
    {
        __LOG_DEBUG_FUNCTION__( function, line, "add close handle[{}:{}]", id, KFAppId::ToString( id ) );

        _close_handle_list[ id ] = _net_server_service->_now_time + delay_time;
        return true;
    }

    bool KFNetServerEngine::BindObjectId( uint64 handle_id, uint64 object_id )
    {
        // 正在关闭队列
        if ( _close_handle_list.find( handle_id ) != _close_handle_list.end() )
        {
            return false;
        }

        // 找不到处理器, 或者正在关闭
        auto handle = _work_handle_list.Find( handle_id );
        if ( handle == nullptr || handle->_is_shutdown )
        {
            return false;
        }

        handle->_object_id = object_id;
        handle->_net_data._id = object_id;
        handle->_net_data._session = handle_id;

        // 删除超时列表
        _bind_timeout_list.erase( handle_id );
        return true;
    }

    std::shared_ptr<KFNetHandle> KFNetServerEngine::RegisterHandle( uint64 trustee_id, uint64 handle_id, uint64 object_id )
    {
        auto handle = _trustee_handle_list.Find( trustee_id );
        if ( handle == nullptr || handle->_is_shutdown || !handle->_is_connected )
        {
            __LOG_ERROR__( "trustee handle[{}:{}] can't find", trustee_id, handle_id );
            return nullptr;
        }

        // 已经在列表中
        if ( _work_handle_list.Find( handle_id ) != nullptr )
        {
            __LOG_ERROR__( "handle[{}:{}] already exist", handle_id, KFAppId::ToString( handle_id ) );
            return nullptr;
        }

        // 加入删除列表( 不能直接删除, 应为在托管队列的消息循环中 )
        _remove_trustee_list.insert( trustee_id );

        // 设置属性, 并加入注册列表中
        handle->_is_trustee = false;
        handle->_object_id = object_id;
        handle->_session_id = handle_id;
        _work_handle_list.Insert( handle_id, handle );
        return handle;
    }

    void KFNetServerEngine::RunRemoveTrusteeHandle()
    {
        if ( _remove_trustee_list.empty() )
        {
            return;
        }

        for ( auto id : _remove_trustee_list )
        {
            _trustee_handle_list.Remove( id );
        }

        _remove_trustee_list.clear();
    }

    void KFNetServerEngine::RunCheckBindTimeout()
    {
        std::list<uint64> remove_list;
        for ( auto iter : _bind_timeout_list )
        {
            if ( _net_server_service->_now_time < iter.second )
            {
                continue;
            }

            remove_list.push_back( iter.first );

            // 已经超时
            auto handle = _trustee_handle_list.Find( iter.first );
            if ( handle != nullptr )
            {
                handle->CloseHandle();
            }
            else
            {
                handle = _work_handle_list.Find( iter.first );
                if ( handle != nullptr )
                {
                    handle->CloseHandle();
                }
            }
        }

        for ( auto handle_id : remove_list )
        {
            _bind_timeout_list.erase( handle_id );
        }
    }

    void KFNetServerEngine::RunCloseHandle()
    {
        for ( auto iter = _close_handle_list.begin(); iter != _close_handle_list.end(); )
        {
            if ( iter->second > _net_server_service->_now_time )
            {
                ++iter;
                continue;
            }

            auto handle = _work_handle_list.Find( iter->first );
            if ( handle != nullptr )
            {
                handle->CloseHandle();
            }

            iter = _close_handle_list.erase( iter );
        }
    }

    void KFNetServerEngine::RunTrusteeMessage( uint64 now_time )
    {
        // 每次取1个消息, 只处理认证消息
        static const uint32 _max_message_count = 1;

        for ( auto& iter : _trustee_handle_list._objects )
        {
            auto handle = iter.second;
            handle->RunUpdate( _max_message_count );
        }
    }

    void KFNetServerEngine::RunHandleMessage( uint64 now_time )
    {
        // 每次取设置消息数量, 防止占用过多的cpu
        for ( auto& iter : _work_handle_list._objects )
        {
            auto handle = iter.second;
            handle->RunUpdate( _handle_message_count );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFNetServerEngine::SendNetMessage( uint32 msg_id, const char* data, uint32 length, uint64 exclude_id /* = 0 */ )
    {
        auto handle = _work_handle_list.First();
        while ( handle != nullptr )
        {
            if ( handle->_object_id != exclude_id )
            {
                handle->SendNetMessage( handle->_object_id, msg_id, data, length );
            }

            handle = _work_handle_list.Next();
        }
    }

    void KFNetServerEngine::SendMessageToName( const std::string& name, uint32 msg_id, const char* data, uint32 length )
    {
        auto handle = _work_handle_list.First();
        while ( handle != nullptr )
        {
            if ( handle->_net_data._name == name )
            {
                handle->SendNetMessage( handle->_object_id, msg_id, data, length );
            }

            handle = _work_handle_list.Next();
        }
    }

    void KFNetServerEngine::SendMessageToType( const std::string& type, uint32 msg_id, const char* data, uint32 length )
    {
        auto handle = _work_handle_list.First();
        while ( handle != nullptr )
        {
            if ( handle->_net_data._type == type )
            {
                handle->SendNetMessage( handle->_object_id, msg_id, data, length );
            }

            handle = _work_handle_list.Next();
        }
    }

    bool KFNetServerEngine::SendNetMessage( uint64 handle_id, uint32 msg_id, const char* data, uint32 length, uint32 delay )
    {
        auto handle = FindNetHandle( handle_id );
        if ( handle == nullptr )
        {
            __LOG_ERROR__( "msg_id[{}] can't find handle[{}]", msg_id, KFAppId::ToString( handle_id ) );
            return false;
        }

        return handle->SendNetMessage( handle_id, msg_id, data, length, delay );
    }

    bool KFNetServerEngine::SendNetMessage( uint64 handle_id, uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint32 delay )
    {
        auto handle = FindNetHandle( handle_id );
        if ( handle == nullptr )
        {
            __LOG_ERROR__( "msg_id[{}] can't find handle[{}]", msg_id, KFAppId::ToString( handle_id ) );
            return false;
        }

        return handle->SendNetMessage( recv_id, msg_id, data, length, delay );
    }
}
