#include "KFNetServerEngine.h"

namespace KFrame
{
    KFNetServerEngine::KFNetServerEngine()
    {
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
            while ( kfhandle != nullptr )
            {
                kfhandle->CloseHandle();
                kfhandle = _trustee_handles.Next();
            }
        }

        {
            auto kfhandle = _kf_handles.First();
            while ( kfhandle != nullptr )
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
        _net_server_services->_now_time = nowtime;
        _net_server_services->_net_event->RunEvent();

        // 删除托管的连接
        RunRemoveTrusteeHandle();

        // 判断托管超时
        RunCheckTrusteeTimeout();

        // 需要关闭的连接
        RunCloseHandle();

        // 处理消息
        RunTrusteeMessage( nowtime );
        RunHandleMessage( nowtime );
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
            handlelist.push_back( kfhandle->_object_id );
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
        kfhandle->SetTrusteeTimeout( _net_server_services->_now_time + 30000 );
    }

    void KFNetServerEngine::OnServerShutDown( const KFEventData* eventdata )
    {
        auto istrustee = reinterpret_cast< uint64 >( eventdata->_data ) == 1 ? true : false;
        if ( istrustee )
        {
            _trustee_handles.Remove( eventdata->_id );
        }
        else
        {
            auto kfhandle = _kf_handles.Find( eventdata->_id );
            if ( kfhandle != nullptr )
            {
                __LOG_DEBUG__( "handle[{}:{}:{}] shutdown ok!", kfhandle->_session_id, kfhandle->_object_id, KFAppID::ToString( kfhandle->_object_id ) );
            }

            _kf_handles.Remove( eventdata->_id );
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
                _server_lost_function( kfhandle->_session_id, kfhandle->_app_name, kfhandle->_app_type );
            }
        }
        else
        {
            kfhandle = _trustee_handles.Find( eventdata->_id );
            if ( kfhandle == nullptr )
            {
                return __LOG_ERROR__( "can't find handle[{}]!", eventdata->_id );
            }
        }

        kfhandle->CloseHandle();
    }

    bool KFNetServerEngine::CloseHandle( uint32 id, uint32 delaytime, const char* function, uint32 line )
    {
        __LOG_DEBUG_FUNCTION__( function, line, "add close handle[{}:{}]!", id, KFAppID::ToString( id ) );

        _close_handles[ id ] = _net_server_services->_now_time + delaytime;
        return true;
    }

    KFNetHandle* KFNetServerEngine::RegisteHandle( uint32 trusteeid, uint32 handleid, uint32 objectid )
    {
        auto kfhandle = _trustee_handles.Find( trusteeid );
        if ( kfhandle == nullptr )
        {
            __LOG_ERROR__( "trustee handle[{}:{}] can't find!", trusteeid, handleid );
            return nullptr;
        }

        // 已经在列表中
        auto findhandle = _kf_handles.Find( handleid );
        if ( findhandle != nullptr )
        {
            __LOG_ERROR__( "handle[{}:{}] already exist!", handleid, KFAppID::ToString( handleid ) );
            return nullptr;
        }

        // 从代理列表中删除
        _remove_trustees[ trusteeid ] = false;

        kfhandle->_is_trustee = false;
        kfhandle->_session_id = handleid;
        kfhandle->_object_id = objectid;
        _kf_handles.Insert( handleid, kfhandle );
        return kfhandle;
    }

    bool KFNetServerEngine::BindObjectId( uint32 handleid, uint32 objectid )
    {
        auto kfhandle = _kf_handles.Find( handleid );
        if ( kfhandle == nullptr )
        {
            return false;
        }

        kfhandle->_object_id = objectid;
        return true;
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
            if ( kfhandle->IsTrusteeTimeout( _net_server_services->_now_time ) )
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
            if ( iter->second > _net_server_services->_now_time )
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

    void KFNetServerEngine::RunTrusteeMessage( uint64 nowtime )
    {
        // 每次取10个消息, 防止占用过多的cpu
        static const uint32 _max_message_count = 10;

        for ( auto& iter : _trustee_handles._objects )
        {
            auto kfhandle = iter.second;
            kfhandle->RunUpdate( _net_function, _max_message_count );
        }
    }

    void KFNetServerEngine::RunHandleMessage( uint64 nowtime )
    {
        // 每次取200个消息, 防止占用过多的cpu
        static const uint32 _max_message_count = 200;

        for ( auto& iter : _kf_handles._objects )
        {
            auto kfhandle = iter.second;
            kfhandle->RunUpdate( _net_function, _max_message_count );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFNetServerEngine::SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 excludeid /* = 0 */ )
    {
        auto kfhandle = _kf_handles.First();
        while ( kfhandle != nullptr )
        {
            if ( kfhandle->_object_id != excludeid )
            {
                kfhandle->SendNetMessage( kfhandle->_object_id, msgid, data, length );
            }

            kfhandle = _kf_handles.Next();
        }
    }

    void KFNetServerEngine::SendMessageToName( const std::string& name, uint32 msgid, const char* data, uint32 length )
    {
        auto kfhandle = _kf_handles.First();
        while ( kfhandle != nullptr )
        {
            if ( kfhandle->_app_name == name )
            {
                kfhandle->SendNetMessage( kfhandle->_object_id, msgid, data, length );
            }

            kfhandle = _kf_handles.Next();
        }
    }

    void KFNetServerEngine::SendMessageToType( const std::string& type, uint32 msgid, const char* data, uint32 length )
    {
        auto kfhandle = _kf_handles.First();
        while ( kfhandle != nullptr )
        {
            if ( kfhandle->_app_type == type )
            {
                kfhandle->SendNetMessage( kfhandle->_object_id, msgid, data, length );
            }

            kfhandle = _kf_handles.Next();
        }
    }

    bool KFNetServerEngine::SendNetMessage( uint32 handleid, uint32 msgid, const char* data, uint32 length )
    {
        KFNetHandle* handle = FindNetHandle( handleid );
        if ( handle == nullptr )
        {
            __LOG_ERROR__( "msgid[{}] can't find handle[{}:{}]!", msgid, handleid, KFAppID::ToString( handleid ) );
            return false;
        }

        return handle->SendNetMessage( handleid, msgid, data, length );
    }

    bool KFNetServerEngine::SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        KFNetHandle* handle = FindNetHandle( handleid );
        if ( handle == nullptr )
        {
            __LOG_ERROR__( "msgid[{}] can't find handle[{}:{}]!", msgid, handleid, KFAppID::ToString( handleid ) );
            return false;
        }

        return handle->SendNetMessage( objectid, msgid, data, length );
    }
}
