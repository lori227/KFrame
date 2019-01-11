#include "KFWorkerMessage.h"

namespace KFrame
{
    KFWorkerMessage::KFWorkerMessage()
    {
        _msgid = 0;
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

    void KFWorkerMessage::CopyFrom( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        _route = route;
        _msgid = msgid;
        _length = length;
        if ( _length > 0 )
        {
            _data = __KF_MALLOC__( char, _length );
            memcpy( _data, data, _length );
        }
    }
}
