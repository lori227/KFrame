#ifndef __KF_DISPLAY_PLUGIN_H__
#define __KF_DISPLAY_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFDisplayPlugin : public KFPlugin
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