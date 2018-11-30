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
            __KF_FREE__( char, _data, _length );
        }
    }

    void KFWorkerMessage::CopyFrom( const KFId& kfid, uint32 msgid, const char* data, uint32 length )
    {
        _kfid = kfid;
        _msgid = msgid;
        _length = length;
        if ( _length > 0 )
        {
            _data = __KF_MALLOC__( char, _length );
            memcpy( _data, data, _length );
        }
    }
}
