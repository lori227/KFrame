#include "KFRunable.hpp"

namespace KFrame
{
    void KFRunable::StartThread( KFThreadFunction& function, const char* file, uint32 line )
    {
        _function = function;
        _file = file;
        _line = line;
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
        if ( _idle_pool.empty() )
        {
            return new KFRunable();
        }

        auto kfrunable = _idle_pool.front();
        _idle_pool.pop_front();
        return kfrunable;
    }

    void KFRunablePool::DestroyRunable( KFRunable* runable )
    {
        _idle_pool.push_back( runable );
    }
}