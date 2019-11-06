#ifndef __KF_EXECUTE_PLUGIN_H__
#define __KF_EXECUTE_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFExecutePlugin : public KFPlugin
    {
    public:
        // 注册
        virtual void Install();

        // 卸载
        virtual void UnInstall();

        // 加载
        virtual void LoadModule();

        // 配置
        virtual void AddConfig();
    };
}


#endif