#ifndef __KF_ZONE_PLUGIN_H__
#define __KF_ZONE_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFZonePlugin : public KFPlugin
    {
    public:
        // 注册
        virtual void Install();

        // 卸载
        virtual void UnInstall();

        // 加载
        virtual void LoadModule();

    };
}


#endif