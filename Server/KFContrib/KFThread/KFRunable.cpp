#include "KFRunable.hpp"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
    KFRunable::~KFRunable()
    {
    }

    void KFRunable::StartThread( KFThreadFunction& function, const char* file, uint32 line )
    {
        _function = function;
        _file = file;
        _line = line;

        // 如果是原来申请的线程, 线程句柄没有释放, 这里释放掉
        _thread.join();
        _thread.start( *this );
    }

    void KFRunable::run()
    {
        _function();

        KFRunablePool::Instance()->DestroyRunable( this );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    KFRunablePool::KFRunablePool()
    {
        _idle_pool.clear();
    }

    KFRunablePool::~KFRunablePool()
    {
        for ( auto runable : _idle_pool )
        {
            delete runable;
        }

        _idle_pool.clear();
    }

    KFRunable* KFRunablePool::CreateRunable()
    {
        //if ( _idle_pool.empty() )
        {
            return new KFRunable();
        }

        auto kfrunable = _idle_pool.front();
        _idle_pool.pop_front();
        return kfrunable;
    }

    void KFRunablePool::DestroyRunable( KFRunable* runable )
    {
        delete runable;
        //_idle_pool.push_back( runable );
    }
}