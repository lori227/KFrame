#include "KFNetLocker.hpp"

namespace KFrame
{
    KFNetLocker::KFNetLocker( uv_mutex_t* uv_mutex )
    {
        _uv_mutex = uv_mutex;
        uv_mutex_lock( _uv_mutex );
    }

    KFNetLocker::~KFNetLocker()
    {
        uv_mutex_unlock( _uv_mutex );
    }
}