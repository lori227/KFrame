#include "KFMutex.h"
#include "Poco/Mutex.h"

namespace KFrame
{
    KFMutex::KFMutex()
    {
        auto mutex = new ( _buff ) Poco::FastMutex();
    }

    KFMutex::~KFMutex()
    {
    }

    void KFMutex::Lock()
    {
        reinterpret_cast< Poco::FastMutex* >( _buff )->lock();
    }

    void KFMutex::Unlock()
    {
        reinterpret_cast< Poco::FastMutex* >( _buff )->unlock();
    }

    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////
    KFLocker::KFLocker( KFMutex& kfmutex ) : _kf_mutex( kfmutex )
    {
        _kf_mutex.Lock();
    }

    KFLocker::~KFLocker()
    {
        _kf_mutex.Unlock();
    }
}
