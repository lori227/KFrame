#ifndef __KF_SERVICES_H__
#define __KF_SERVICES_H__

#include "KFrame.h"

namespace KFrame
{
    class KFApplication;
    class KFServices : public KFSingleton<KFServices>
    {
    public:
        // 构造
        KFServices() = default;
        ~KFServices() = default;

        // run
        void Run();

        // 初始化服务
        bool InitService( KFApplication* application, StringMap& params );

        // 自动更新
        void RunUpdate();

        // 关闭服务
        void ShutDown();

        // 是否已经关闭
        bool IsShutDown();

    protected:
        // 设置游戏的显示和运行时间
        void UpdateTime();

    protected:
        // 进程
        KFApplication* _application = nullptr;
    };
}
#endif
