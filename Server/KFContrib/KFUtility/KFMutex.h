#ifndef __KF_MUTEX_H__
#define __KF_MUTEX_H__

namespace KFrame
{
    class KFMutex
    {
    public:
        KFMutex();
        ~KFMutex();

        void Lock();
        void Unlock();

    private:
        char _buff[ 128 ];
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFLocker
    {
    public:
        KFLocker( KFMutex& kfmutex );
        ~KFLocker();

    private:
        KFMutex& _kf_mutex;
    };
}

#endif
