#include "KFThread.h"
#include "KFRunnable.hpp"

namespace KFrame
{
    ThreadId KFThread::GetThreadID()
    {
        return std::this_thread::get_id();
    }

    ThreadId KFThread::CreateThread( KFThreadFunction& function, const char* file, uint32 line )
    {
        auto runnable = KFRunnablePool::Instance()->Create( function, file, line );
        return runnable->GetID();
    }

    void KFThread::Sleep( uint32 milliseconds )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
    }
}