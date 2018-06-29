#include "KFNetEvent.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
    KFNetEvent::KFNetEvent()
    {
        _kf_connect_function = nullptr;
        _kf_disconnect_function = nullptr;
        _kf_failed_function = nullptr;
        _kf_shut_function = nullptr;
    }

    KFNetEvent::~KFNetEvent()
    {
    }

    void KFNetEvent::InitEvent( uint32 maxcount )
    {
        _net_event_data.InitQueue( maxcount );
    }

    void KFNetEvent::AddEvent( uint32 type, uint32 id, void* data, const char* describe /* = "" */, int32 code /* = 0 */ )
    {
        auto eventdata = __KF_CREATE_BATCH__( KFEventData, 50 );

        eventdata->_id = id;
        eventdata->_type = type;
        eventdata->_code = code;
        eventdata->_describe = describe;
        eventdata->_data = data;
        auto ok = _net_event_data.PushObject( eventdata );
        if ( !ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] event queue full!", __FUNCTION__ );
        }
    }

    void KFNetEvent::RunEvent()
    {
        auto eventdata = _net_event_data.PopObject();
        while ( eventdata != nullptr )
        {
            HandleBindEventFunction( eventdata );
            __KF_DESTROY__( KFEventData, eventdata );

            eventdata = _net_event_data.PopObject();
        }
    }

    void KFNetEvent::ShutEvent()
    {
        _kf_connect_function = nullptr;
        _kf_disconnect_function = nullptr;
        _kf_failed_function = nullptr;
        _kf_shut_function = nullptr;

        _net_event_data.ClearObject();
    }

    void KFNetEvent::HandleBindEventFunction( const KFEventData* eventdata )
    {
        switch ( eventdata->_type )
        {
        case KFNetDefine::ConnectEvent:
        {
            if ( _kf_connect_function != nullptr )
            {
                _kf_connect_function( eventdata );
            }
        }
        break;
        case KFNetDefine::DisconnectEvent:
        {
            if ( _kf_disconnect_function != nullptr )
            {
                _kf_disconnect_function( eventdata );
            }
        }
        break;
        case KFNetDefine::FailedEvent:
        {
            if ( _kf_failed_function != nullptr )
            {
                _kf_failed_function( eventdata );
            }
        }
        break;
        case KFNetDefine::ShutEvent:
        {
            if ( _kf_shut_function != nullptr )
            {
                _kf_shut_function( eventdata );
            }
        }
        break;
        default:
            break;
        }
    }
}
