#ifndef __KF_RELATION_DATABASE_PLUGIN_H__
#define __KF_RELATION_DATABASE_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFRelationDatabasePlugin : public KFPlugin
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