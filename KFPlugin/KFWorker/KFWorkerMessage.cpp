#include "KFWorkerMessage.hpp"

namespace KFrame
{
    KFWorkerMessage::KFWorkerMessage()
    {
        _msg_id = 0;
        _data = nullptr;
        _length = 0;
    }

    KFWorkerMessage::~KFWorkerMessage()
    {
        if ( _data != nullptr )
        {
            __KF_FREE__( char, _data );
        }
    }

    void KFWorkerMessage::CopyFrom( const Route& route, uint32 msg_id, const char* data, uint32 length )
    {
        _route = route;
        _msg_id = msg_id;
        _length = length;
        if ( _length > 0 )
        {
            _data = __KF_MALLOC__( char, _length );
            memcpy( _data, data, _length );
        }
    }
}
