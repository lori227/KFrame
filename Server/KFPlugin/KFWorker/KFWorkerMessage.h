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

        void CopyFrom( const KFId& kfid, uint32 msgid, const char* data, uint32 length );

    public:
        KFId _kfguid;
        uint32 _msgid;
        char* _data;
        uint32 _length;
    };
}


#endif