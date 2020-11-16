#ifndef __KF_RUNABLE_H__
#define __KF_RUNABLE_H__

#include "KFrame.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    class KFRunable : public Poco::Runnable
    {
    public:
        ~KFRunable();

        // 开启线程
        void StartThread( KFThreadFunction& function, const char* file, uint32 line );

        // poco run
        virtual void run();

        // 获得线程id
        uint32 GetID();
    private:
        // 线程
        Poco::Thread _thread;

        // 回调函数
        std::string _file;
        uint32 _line = 0;
        KFThreadFunction _function;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    class KFRunablePool : public KFSingleton< KFRunablePool >
    {
    public:
        KFRunablePool();
        ~KFRunablePool();

        // 创建线程
        KFRunable* CreateRunable();

        // 释放线程
        void DestroyRunable( KFRunable* runable );

    private:
        // 空闲的线程
        std::list< KFRunable* > _idle_pool;
    };
}

#endif
