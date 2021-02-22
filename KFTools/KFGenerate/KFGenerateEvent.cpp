#include "KFGenerateEvent.h"

namespace KFrame
{
    void KFGenerateEvent::AddEvent( uint32 type, uint32 intparam, const std::string& strparam )
    {
        auto eventdata = __KF_NEW__( EventData );
        eventdata->_type = type;
        eventdata->_int_param = intparam;
        eventdata->_str_param = strparam;

        KFLocker locker( _mutex );
        _event_list.push_back( eventdata );
    }

    void KFGenerateEvent::ExecuteEvent()
    {
        std::list< EventData* > eventlist;
        {
            KFLocker locker( _mutex );
            eventlist.swap( _event_list );
        }

        if ( eventlist.empty() )
        {
            return;
        }

        for ( auto eventdata : eventlist )
        {
            auto kffunction = _event_function.Find( eventdata->_type );
            if ( kffunction != nullptr )
            {
                kffunction->Call( eventdata );
            }
            __KF_DELETE__( EventData, eventdata );
        }
    }
}
