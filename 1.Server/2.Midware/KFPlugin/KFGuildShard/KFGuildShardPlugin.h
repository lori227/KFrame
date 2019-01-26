#ifndef __KF_GUILD_SHARD_PLUGIN_H__
#define __KF_GUILD_SHARD_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFGuildShardPlugin : public KFPlugin
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