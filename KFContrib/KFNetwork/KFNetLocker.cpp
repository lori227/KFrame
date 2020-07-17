#include "KFNetLocker.hpp"

namespace KFrame
{
    KFNetLocker::KFNetLocker( uv_mutex_t* uvmutex )
    {
        _uv_mutex = uvmutex;
        uv_mutex_lock( _uv_mutex );
    }

    KFNetLocker::~KFNetLocker()
    {
        uv_mutex_unlock( _uv_mutex );
    }
}