#ifndef __KF_RUNNABLE_H__
#define __KF_RUNNABLE_H__

#include "KFMap.h"
#include "KFMutex.h"
#include "KFSingleton.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    class KFRunnable : public std::enable_shared_from_this<KFRunnable>
    {
    public:
        ~KFRunnable();

        // 开启线程
        ThreadId Start( KFThreadFunction& function, const char* file, uint32 line );

        // 获得线程id
        ThreadId GetID();

    protected:
        // run
        void Run();

    private:
        // 线程
        std::shared_ptr<std::thread> _thread = nullptr;

        // 回调函数
        uint32 _line = 0;
        std::string _file;
        KFThreadFunction _function = nullptr;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    class KFRunnablePool : public KFSingleton<KFRunnablePool>
    {
    public:
        KFRunnablePool();
        ~KFRunnablePool();

        // 创建线程
        std::shared_ptr<KFRunnable> Create( KFThreadFunction& function, const char* file, uint32 line );

        // 释放线程
        void Destroy( std::shared_ptr<KFRunnable> runnable );

    private:
        // 线程锁
        KFMutex _mutex;

        // 正在运行的线程( 保持生命周期 )
        KFHashMap<ThreadId, KFRunnable> _runnable_list;
    };
}

#endif
