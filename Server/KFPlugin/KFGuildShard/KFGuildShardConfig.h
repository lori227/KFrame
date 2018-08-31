#ifndef __KF_GUILD_SHARD_CONFIG_H__
#define __KF_GUILD_SHARD_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    namespace KFGuildShardEnum
    {
        enum EConstDefine
        {
            GuildMaster = 1,

            GuildMember = 2,


        };
    };

    class KFGuildShardConfig : public KFConfig, public KFSingleton< KFGuildShardConfig >
    {
    public:
        KFGuildShardConfig();
        ~KFGuildShardConfig();

        bool LoadConfig();

    public:

        //每日清除被敬酒信息的时间配置
        uint32 _clean_hour;
        uint32 _clean_minute;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_public_shard_config = KFGuildShardConfig::Instance();
}

#endif