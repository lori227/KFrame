#include "KFMutex.h"

namespace KFrame
{
    void KFMutex::Lock()
    {
        _mutex.lock();
    }

    void KFMutex::Unlock()
    {
        _mutex.unlock();
    }

    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////
    KFLocker::KFLocker( KFMutex& mutex ) : _mutex( mutex )
    {
        _mutex.Lock();
    }

    KFLocker::~KFLocker()
    {
        _mutex.Unlock();
    }
}
