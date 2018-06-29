#ifndef __KF_SERVICES_H__
#define __KF_SERVICES_H__

#include "KFrame.h"
#include "KFTime/KFTimer.h"

namespace KFrame
{
    class KFApplication;
    class KFServices : public KFSingleton< KFServices >
    {
    public:
        // 构造
        KFServices();
        ~KFServices();

        // run
        void Run();

        // 初始化服务
        virtual bool InitService( KFApplication* application, const std::vector< std::string >& params );

        // 自动更新
        virtual void RunUpdate();

        // 关闭服务
        virtual void ShutDown();

    protected:
        ////////////////////////////////////////////////////////////////////////////
        // 初始化定时器
        void InitLogMemoryTimer();

        // 打印内存信息
        void PrintLogMemory();

    protected:
        // 进程
        KFApplication* _application;

        // 打印定时器
        KFClockTimer _memory_timer;
    };
}
#endif
