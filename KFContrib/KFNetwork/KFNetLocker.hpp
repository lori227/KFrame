#ifndef __NET_LOCKER_H__
#define __NET_LOCKER_H__

#include "uv.h"

namespace KFrame
{
    class KFNetLocker
    {
    public:
        KFNetLocker( uv_mutex_t* uv_mutex );
        ~KFNetLocker();

    private:
        uv_mutex_t* _uv_mutex;
    };
}

#endif