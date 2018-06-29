#include "KFThread.h"
#include "KFRunable.hpp"

namespace KFrame
{
    uint32 KFThread::GetThreadID()
    {
        uint32 threadid = Poco::Thread::currentTid();
        return threadid;
    }

    void KFThread::CreateThread( KFThreadFunction& function, const char* file, uint32 line )
    {
        auto kfrunable = KFRunablePool::Instance()->CreateRunable();
        kfrunable->StartThread( function, file, line );
    }

    void KFThread::Sleep( uint32 milliseconds )
    {
        Poco::Thread::sleep( 1 );
    }
}