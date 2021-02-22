#ifndef __KF_THREAD_H__
#define __KF_THREAD_H__

#include "KFDefine.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    class KFThread
    {
    public:
        // 当前线程id
        static ThreadId GetThreadID();

        // 创建线程
        template<class T>
        static ThreadId CreateThread( T* object, void( T::*handle )(), const char* file, uint32 line )
        {
            KFThreadFunction function = std::bind( handle, object );
            return CreateThread( function, file, line );
        }

        // 睡眠
        static void Sleep( uint32 milliseconds );

    protected:
        // 创建线程
        static ThreadId CreateThread( KFThreadFunction& function, const char* file, uint32 line );
    };


}

#endif
