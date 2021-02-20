#include "KFThread.h"
#include "KFRunable.hpp"
namespace KFrame
{
    uint32 KFThread::GetThreadID()
    {
#if __KF_SYSTEM__ == __KF_WIN__
        uint32 threadid = Poco::Thread::currentTid();
        return threadid;
#else
        auto thread = Poco::Thread::currentTid();
        return thread->__sig;
#endif
    }

    uint32 KFThread::CreateThread( KFThreadFunction& function, const char* file, uint32 line )
    {
        auto kfrunable = KFRunablePool::Instance()->CreateRunable();
        kfrunable->StartThread( function, file, line );
        return kfrunable->GetID();
    }

    void KFThread::Sleep( uint32 milliseconds )
    {
        Poco::Thread::sleep( 1 );
    }
}