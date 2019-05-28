#include "KFNetMessage.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    KFNetMessage::KFNetMessage( uint32 length )
    {
        _head._length = length;
        if ( _head._length > 0 )
        {
            _data = __KF_MALLOC__( int8, length );
        }
    }

    KFNetMessage::~KFNetMessage()
    {
        if ( _data != nullptr )
        {
            __KF_FREE__( int8, _data );
        }

        _data = nullptr;
        _head._length = 0;
    }

    uint32 KFNetMessage::HeadLength()
    {
        return sizeof( _head );
    }

    KFNetMessage* KFNetMessage::Create( uint32 length )
    {
        return __KF_NEW__( KFNetMessage, length );
    }

    void KFNetMessage::Release()
    {
        __KF_DELETE__( KFNetMessage, this );
    }

    void KFNetMessage::CopyData( const int8* data, uint32 length )
    {
        _head._length = length;
        if ( length == 0 || data == nullptr )
        {
            return;
        }

        memcpy( _data, data, length );
    }

    void KFNetMessage::CopyFrom( KFNetMessage* message )
    {
        _head = message->_head;
        if ( message->_head._length > 0 )
        {
            CopyData( message->_data, message->_head._length );
        }
    }

    void KFNetMessage::CopyFrom( const Route& route, uint32 msgid, const int8* data, uint32 length )
    {
        _head._route = route;
        _head._msgid = msgid;
        CopyData( data, length );
    }
}
