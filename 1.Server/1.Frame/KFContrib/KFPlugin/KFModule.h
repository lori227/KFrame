#ifndef __KF_MODULE_H__
#define __KF_MODULE_H__

#include "KFDefine.h"
#include "KFInterface.h"

namespace KFrame
{
    class KFPlugin;
    class KFModule : public KFInterface
    {
    public:
        KFModule();
        ~KFModule();

        // 安装
        virtual void Install() {}

        // 卸载
        virtual void UnInstall() {}

        // 初始化
        virtual void InitModule() {}

        // 加载配置
        virtual void LoadConfig() {}
        virtual void AfterLoad() {}

        // 执行
        virtual void BeforeRun() {}
        virtual void OnceRun() {}
        virtual void Run() {}
        virtual void AfterRun() {}

        // 关闭
        virtual void BeforeShut() {}
        virtual void ShutDown() {}
        virtual void AfterShut() {}
        //////////////////////////////////////////////////////////////

    public:
        // 插件
        KFPlugin* _kf_plugin;
    };
}

#endif