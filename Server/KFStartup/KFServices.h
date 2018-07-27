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
        bool InitService( KFApplication* application, const std::vector< std::string >& params );

        // 自动更新
        void RunUpdate();

        // 关闭服务
        void ShutDown();

        // 是否已经关闭
        bool IsShutDown();

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
