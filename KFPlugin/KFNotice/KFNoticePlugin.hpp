#ifndef __KF_NOTICE_PLUGIN_H__
#define __KF_NOTICE_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFNoticePlugin : public KFPlugin
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