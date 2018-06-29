#ifndef __KF_THREAD_H__
#define __KF_THREAD_H__

#include "KFInclude.h"

namespace KFrame
{
    typedef std::function< void() > KFThreadFunction;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class KFThread
    {
    public:
        // 当前线程id
        static uint32 GetThreadID();

        // 创建线程
        template< class T >
        static void CreateThread( T* object, void( T::*handle )(), const char* file, uint32 line )
        {
            KFThreadFunction function = std::bind( handle, object );
            CreateThread( function, file, line );
        }

        // 睡眠
        static void Sleep( uint32 milliseconds );

    protected:
        // 创建线程
        static void CreateThread( KFThreadFunction& function, const char* file, uint32 line );
    };


}

#endif
