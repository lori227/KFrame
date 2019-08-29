#ifndef __KF_DEPLOY_CLIENT_PLUGIN_H__
#define __KF_DEPLOY_CLIENT_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFDeployClientPlugin : public KFPlugin
    {
    public:
        // 注册
        virtual void Install();

        // 卸载
        virtual void UnInstall();

        // 加载模块
        virtual void LoadModule();

        // 添加配置文件
    };
}


#endif