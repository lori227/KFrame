#ifndef __KF_MONGO_PLUGIN_H__
#define __KF_MONGO_PLUGIN_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMongoPlugin : public KFPlugin
    {
    public:
        // 安装
        virtual void Install();

        // 卸载
        virtual void UnInstall();

        // 加载模块
        virtual void LoadModule();

        // 添加配置
        virtual void AddConfig();
    };
}


#endif