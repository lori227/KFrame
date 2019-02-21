#ifndef __KF_RANK_MASTER_PLUGIN_H__
#define __KF_RANK_MASTER_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFRankMasterPlugin : public KFPlugin
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