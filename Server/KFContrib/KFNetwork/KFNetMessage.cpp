#include "KFNetMessage.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    KFNetMessage::KFNetMessage()
    {
        _msgid = 0;
        _length = 0;
        _child = 0;
        _data = nullptr;
    }

    KFNetMessage::~KFNetMessage()
    {
        FreeData();
    }

    void KFNetMessage::CopyData( const int8* data, uint32 length )
    {
        _length = length;
        if ( length == 0 || data == nullptr )
        {
            return;
        }

        memcpy( _data, data, length );
    }

    KFNetMessage* KFNetMessage::Create( uint32 length )
    {
        KFNetMessage* message = __KF_NEW__( KFNetMessage );
        message->MallocData( length );
        return message;
    }

    void KFNetMessage::Release()
    {
        FreeData();
        __KF_DELETE__( KFNetMessage, this );
    }

    void KFNetMessage::CopyFrom( KFNetMessage* message )
    {
        _kfid = message->_kfid;
        _msgid = message->_msgid;
        _length = message->_length;
        _child = message->_child;
        if ( message->_length > 0 )
        {
            CopyData( message->_data, message->_length );
        }
    }

    void KFNetMessage::CopyFrom( const KFId& kfid, uint32 msgid, const int8* data, uint32 length )
    {
        _kfid = kfid;
        _msgid = msgid;
        CopyData( data, length );
    }

    void KFNetMessage::MallocData( uint32 length )
    {
        _length = length;
        if ( _length > 0 )
        {
            _data = __KF_MALLOC__( int8, length );
        }
    }

    void KFNetMessage::FreeData()
    {
        if ( _data != nullptr )
        {
            __KF_FREE__( int8, _data );
        }

        _data = nullptr;
        _length = 0;
    }
}
