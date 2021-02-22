#include "KFRunnable.hpp"

namespace KFrame
{
    KFRunnable::~KFRunnable()
    {
    }

    ThreadId KFRunnable::Start( KFThreadFunction& function, const char* file, uint32 line )
    {
        _file = file;
        _line = line;
        _function = function;
        _thread = __MAKE_SHARED__( std::thread, std::bind( &KFRunnable::Run, this ) );
        return _thread->get_id();
    }

    void KFRunnable::Run()
    {
        try
        {
            _function();
        }
        catch ( ... )
        {

        }

        KFRunnablePool::Instance()->Destroy( shared_from_this() );
    }

    ThreadId KFRunnable::GetID()
    {
        return _thread->get_id();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    KFRunnablePool::KFRunnablePool()
    {

    }

    KFRunnablePool::~KFRunnablePool()
    {
        KFLocker lock( _mutex );
        _idle_list.clear();
        _runnable_list.Clear();
    }

    std::shared_ptr<KFRunnable> KFRunnablePool::Create( KFThreadFunction& function, const char* file, uint32 line )
    {
        KFLocker lock( _mutex );
        std::shared_ptr<KFRunnable> runnable = nullptr;
        if ( !_idle_list.empty() )
        {
            runnable = _idle_list.front();
            _idle_list.pop_front();
        }
        else
        {
            runnable = __MAKE_SHARED__( KFRunnable );
        }

        runnable->Start( function, file, line );
        _runnable_list.Insert( runnable->GetID(), runnable );
        return runnable;
    }

    void KFRunnablePool::Destroy( std::shared_ptr<KFRunnable> runnable )
    {
        KFLocker lock( _mutex );
        _idle_list.push_back( runnable );
        _runnable_list.Remove( runnable->GetID() );
    }
}