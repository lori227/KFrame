#include "KFNetEvent.hpp"

namespace KFrame
{
    void KFNetEvent::InitEvent( uint32 max_count )
    {
        _event_data_queue.InitQueue( max_count, max_count );
    }

    void KFNetEvent::AddEvent( uint32 type, uint64 id, std::shared_ptr<void> data, const char* describe /* = "" */, int32 code /* = 0 */ )
    {
        auto event_data = __MAKE_SHARED__( KFNetEventData );

        event_data->_id = id;
        event_data->_type = type;
        event_data->_code = code;
        event_data->_data = data;
        event_data->_describe = describe;
        _event_data_queue.Push( event_data );
    }

    void KFNetEvent::RunEvent()
    {
        auto event_data = _event_data_queue.Pop();
        while ( event_data != nullptr )
        {
            HandleBindEventFunction( event_data );
            event_data = _event_data_queue.Pop();
        }
    }

    void KFNetEvent::ShutEvent()
    {
        _event_data_queue.Clear();
        _event_function_list.Clear();
    }

    void KFNetEvent::HandleBindEventFunction( std::shared_ptr<KFNetEventData>& event_data )
    {
        auto function = _event_function_list.Find( event_data->_type );
        if ( function == nullptr )
        {
            return;
        }

        function->Call( event_data );
    }
}
