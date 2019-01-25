#ifndef __KF_WORKER_MESSAGE_H__
#define __KF_WORKER_MESSAGE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFWorkerMessage
    {
    public:
        KFWorkerMessage( );
        ~KFWorkerMessage();

        void CopyFrom( const Route& route, uint32 msgid, const char* data, uint32 length );

    public:
        Route _route;
        uint32 _msgid;
        char* _data;
        uint32 _length;
    };
}


#endif