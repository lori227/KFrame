#ifndef __KF_MYSQL_PLUGIN_H__
#define __KF_MYSQL_PLUGIN_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMySQLPlugin : public KFPlugin
    {
    public:
        // 安装
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