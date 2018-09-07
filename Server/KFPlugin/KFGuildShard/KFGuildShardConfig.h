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
            GuildViceMaster = 2,
            GuildMember = 3,

        };
    };

    class KFGuildSetting
    {
    public:
        KFGuildSetting()
        {

        }
    public:
        // 等级
        uint32 _level;

        // 最大成员数
        uint32 _max_member;

        // 升级所需的活跃度
        uint32 _upgrade_activeness;

        // 降级活跃度
        uint32 _degrade_activeness;

        // 最大日志
        uint32 _max_log;

        // 最大申请列表长度
        uint32 _max_applylist;

    };

    class KFGuildShardConfig : public KFConfig, public KFSingleton< KFGuildShardConfig >
    {
    public:
        KFGuildShardConfig();
        ~KFGuildShardConfig();

        bool LoadConfig();

        const KFGuildSetting* FindGuildSetting( uint32 level ) const;

        inline uint32 GetGuildPage()
        {
            return _max_guildlist_page;
        }

    protected:
        void AddGuildSetting( KFGuildSetting* kfsetting );
    public:
        // 排行榜配置列表
        KFMap< uint32, uint32, KFGuildSetting > _kf_guild_setting;
        // 显示属性
        VectorString _show_data;
        uint32 _max_guildlist_page;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_guild_shard_config = KFGuildShardConfig::Instance();
}

#endif