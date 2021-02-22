 #ifndef __KF_MUTEX_H__
#define __KF_MUTEX_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFMutex final
    {
    public:
        void Lock();
        void Unlock();

    private:
        std::mutex _mutex;
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFLocker final
    {
    public:
        KFLocker( KFMutex& mutex );
        ~KFLocker();

    private:
        KFMutex& _mutex;
    };
}

#endif
