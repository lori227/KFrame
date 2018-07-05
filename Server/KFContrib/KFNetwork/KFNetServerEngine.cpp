#include "KFNetServerEngine.h"

namespace KFrame
{
    KFNetServerEngine::KFNetServerEngine()
    {
        _now_time = 0;
        _net_function = nullptr;
        _net_server_services = nullptr;
        _server_lost_function = nullptr;
    }

    KFNetServerEngine::~KFNetServerEngine()
    {
        __KF_DELETE__( _net_server_services );
    }

    ////////////////////////////////////////////////////
    void KFNetServerEngine::InitEngine( uint32 maxqueuesize )
    {
        _net_server_services = __KF_NEW__( KFNetServerServices );
        _net_server_services->InitServices( 10000, maxqueuesize );
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        _net_server_services->_net_event->BindConnectFunction( this, &KFNetServerEngine::OnServerConnected );
        _net_server_services->_net_event->BindShutFunction( this, &KFNetServerEngine::OnServerShutDown );
        _net_server_services->_net_event->BindDisconnectFunction( this, &KFNetServerEngine::OnServerDisconnect );
    }

    int32 KFNetServerEngine::StartEngine( const std::string& ip, uint32 port, uint32 maxconnect, uint32 timeout )
    {

        KFNetSetting kfsetting;
        kfsetting._ip = ip;
        kfsetting._port = port;
        kfsetting._max_connect = maxconnect;
        kfsetting._timeout = timeout;
        return _net_server_services->StartServices( &kfsetting );
    }

    void KFNetServerEngine::ShutEngine()
    {
        {
            auto kfhandle = _trustee_handles.First();
            while ( true )
            {
                kfhandle->CloseHandle();
                kfhandle = _trustee_handles.Next();
            }
        }

        {
            auto kfhandle = _kf_handles.First();
            while ( true )
            {
                kfhandle->CloseHandle();
                kfhandle = _kf_handles.Next();
            }
        }

        _net_server_services->ShutServices();
        _net_server_services->_net_event->ShutEvent();
    }

    void KFNetServerEngine::RunEngine( uint64 nowtime )
    {
        _now_time = nowtime;
        _net_server_services->_net_event->RunEvent();

        // 删除托管的连接
        RunRemoveTrusteeHandle();

        // 判断托管超时
        RunCheckTrusteeTimeout();

        // 需要关闭的连接
        RunCloseHandle();

        // 处理消息
        RunTrusteeMessage();
        RunHandleMessage();
    }

    uint32 KFNetServerEngine::GetHandleCount()
    {
        return _kf_handles.Size();
    }

    void KFNetServerEngine::GetHandleList( std::list<uint32>& handlelist )
    {
        auto kfhandle = _kf_handles.First();
        while ( kfhandle != nullptr )
        {
            handlelist.push_back( kfhandle->_id );
            kfhandle = _kf_handles.Next();
        }
    }

    KFNetHandle* KFNetServerEngine::FindNetHandle( uint32 handleid )
    {
        return _kf_handles.Find( handleid );
    }

    const std::string& KFNetServerEngine::GetHandleIp( uint32 handleid )
    {
        auto kfhandle = FindNetHandle( handleid );
        if ( kfhandle == nullptr )
        {
            return _invalid_str;
        }

        return kfhandle->_remote_ip;
    }

    void KFNetServerEngine::OnServerConnected( const KFEventData* eventdata )
    {
        // 加入托管列表
        auto kfhandle = reinterpret_cast< KFNetHandle* >( eventdata->_data );
        _trustee_handles.Insert( eventdata->_id, kfhandle );

        // 设置托管超时时间
        kfhandle->SetTrusteeTimeout( _now_time + 60000 );
    }

    void KFNetServerEngine::OnServerShutDown( const KFEventData* eventdata )
    {
        auto ok = _kf_handles.Remove( eventdata->_id );
        if ( !ok )
        {
            ok = _trustee_handles.Remove( eventdata->_id );
        }

        if ( ok )
        {
            KFLogger::LogNet( KFLogger::Debug, "[%s] handle[%u] remove ok!",
                              __FUNCTION__, eventdata->_id );
        }
        else
        {
            KFLogger::LogNet( KFLogger::Debug, "[%s] handle[%u] remove failed!",
                              __FUNCTION__, eventdata->_id );
        }
    }

    void KFNetServerEngine::OnServerDisconnect( const KFEventData* eventdata )
    {
        auto kfhandle = _kf_handles.Find( eventdata->_id );
        if ( kfhandle != nullptr )
        {
            // 断开连接
            if ( _server_lost_function != nullptr )
            {
                _server_lost_function( kfhandle->_app_id, kfhandle->_app_name, kfhandle->_app_type );
            }
        }
        else
        {
            kfhandle = _trustee_handles.Find( eventdata->_id );
            if ( kfhandle == nullptr )
            {
                return KFLogger::LogNet( KFLogger::Error, "[%s] can't find handle[%u]!",
                                         __FUNCTION__, eventdata->_id );
            }
        }

        kfhandle->CloseHandle();
    }

    bool KFNetServerEngine::CloseHandle( uint32 id, uint32 delaytime, const char* function, uint32 line )
    {
        _close_handles[ id ] = _now_time + delaytime;
        KFLogger::LogNet( KFLogger::Debug, "[%s:%u] add close handle[%u]!", function, line, id );

        return true;
    }

    KFNetHandle* KFNetServerEngine::RegisteHandle( uint32 trusteeid, uint32 handleid )
    {
        auto kfhandle = _trustee_handles.Find( trusteeid );
        if ( kfhandle == nullptr )
        {
            KFLogger::LogNet( KFLogger::Error, "[%s] trustee handle[%u:%u] can't find!",
                              __FUNCTION__, trusteeid, handleid );
            return nullptr;
        }

        // 已经在列表中
        auto findhandle = _kf_handles.Find( handleid );
        if ( findhandle != nullptr )
        {
            KFLogger::LogNet( KFLogger::Error, "[%s] handle[%u] already exist!",
                              __FUNCTION__, handleid );
            return nullptr;
        }

        // 从代理列表中删除
        _remove_trustees[ trusteeid ] = false;
        kfhandle->SetID( handleid );

        _kf_handles.Insert( handleid, kfhandle );
        return kfhandle;
    }

    void KFNetServerEngine::RunRemoveTrusteeHandle()
    {
        if ( _remove_trustees.empty() )
        {
            return;
        }

        for ( auto iter : _remove_trustees )
        {
            _trustee_handles.Remove( iter.first, iter.second );
        }

        _remove_trustees.clear();
    }

    void KFNetServerEngine::RunCheckTrusteeTimeout()
    {
        auto kfhandle = _trustee_handles.First();
        while ( kfhandle != nullptr )
        {
            if ( kfhandle->IsTrusteeTimeout( _now_time ) )
            {
                kfhandle->CloseHandle();
            }

            kfhandle = _trustee_handles.Next();
        }
    }

    void KFNetServerEngine::RunCloseHandle()
    {
        for ( auto iter = _close_handles.begin(); iter != _close_handles.end(); )
        {
            if ( iter->second > _now_time )
            {
                ++iter;
                continue;
            }

            auto kfhandle = _kf_handles.Find( iter->first );
            if ( kfhandle != nullptr )
            {
                kfhandle->CloseHandle();
            }

            iter = _close_handles.erase( iter );
        }
    }

    void KFNetServerEngine::RunTrusteeMessage()
    {
        // 每次取10个消息, 防止占用过多的cpu
        static const uint32 _max_message_count = 10;

        for ( auto& iter : _trustee_handles._objects )
        {
            auto kfhandle = iter.second;

            auto messagecount = 0u;
            auto message = kfhandle->PopNetMessage();
            while ( message != nullptr )
            {
                _net_function( message->_guid, message->_msgid, message->_data, message->_length );

                // 每次处理200个消息
                ++messagecount;
                if ( messagecount >= _max_message_count )
                {
                    break;
                }
                message = kfhandle->PopNetMessage();
            }
        }
    }

    void KFNetServerEngine::RunHandleMessage()
    {
        // 每次取200个消息, 防止占用过多的cpu
        static const uint32 _max_message_count = 200;

        for ( auto& iter : _kf_handles._objects )
        {
            auto kfhandle = iter.second;

            auto messagecount = 0u;
            auto message = kfhandle->PopNetMessage();
            while ( message != nullptr )
            {
                _net_function( message->_guid, message->_msgid, message->_data, message->_length );

                // 每次处理200个消息
                ++messagecount;
                if ( messagecount >= _max_message_count )
                {
                    break;
                }
                message = kfhandle->PopNetMessage();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFNetServerEngine::SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 excludeid /* = 0 */ )
    {
        auto kfhandle = _kf_handles.First();
        while ( kfhandle != nullptr )
        {
            if ( kfhandle->_id != excludeid )
            {
                kfhandle->SendNetMessage( kfhandle->_id, msgid, data, length );
            }

            kfhandle = _kf_handles.Next();
        }
    }

    void KFNetServerEngine::SendNetMessage( const std::string& name, uint32 msgid, const char* data, uint32 length )
    {
        auto kfhandle = _kf_handles.First();
        while ( kfhandle != nullptr )
        {
            if ( kfhandle->_app_name == name )
            {
                kfhandle->SendNetMessage( kfhandle->_id, msgid, data, length );
            }

            kfhandle = _kf_handles.Next();
        }
    }

    bool KFNetServerEngine::SendNetMessage( uint32 handleid, uint32 msgid, const char* data, uint32 length )
    {
        KFNetHandle* handle = FindNetHandle( handleid );
        if ( handle == nullptr )
        {
            KFLogger::LogNet( KFLogger::Error, "[%s] msgid[%u] can't find handle[%u]!",
                              __FUNCTION__, msgid, handleid );
            return false;
        }

        return handle->SendNetMessage( handleid, msgid, data, length );
    }

    bool KFNetServerEngine::SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        KFNetHandle* handle = FindNetHandle( handleid );
        if ( handle == nullptr )
        {
            KFLogger::LogNet( KFLogger::Error, "[%s] msgid[%u] can't find handle[%u]!",
                              __FUNCTION__, msgid, handleid );

            return false;
        }

        return handle->SendNetMessage( objectid, msgid, data, length );
    }
}
