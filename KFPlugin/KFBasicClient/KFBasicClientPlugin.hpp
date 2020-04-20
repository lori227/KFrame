#ifndef __KF_BASIC_CLIENT_PLUGIN_H__
#define __KF_BASIC_CLIENT_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFBasicClientPlugin : public KFPlugin
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