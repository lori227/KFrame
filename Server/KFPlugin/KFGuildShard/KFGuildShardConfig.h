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

        enum EnumTitlePowerEnum
        {
            Mask_Invite_Data = 1 << 1,		// 邀请权力
            Mask_Review_Data = 1 << 2,		// 审批权力
            Mask_Dissolve_Data = 1 << 3,	// 解散帮派权力
            Mask_Dismissal_Data = 1 << 4,	// 解雇成员(只能解雇职位比这身低的)
            Mask_Appoint_Data = 1 << 5,		// 任命权力(只能任命职位比这身低的)
            Mask_Modify_Data = 1 << 6,		// 修改帮派徽章等
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

        // 是否拥有某项权力
        bool IsOwnPower( uint32 title, uint32 datamask ) const;

    protected:
        void AddGuildSetting( KFGuildSetting* kfsetting );
    public:
        // 配置列表
        KFMap< uint32, uint32, KFGuildSetting > _kf_guild_setting;
        // 显示属性
        VectorString _show_data;
        uint32 _max_guildlist_page;

        // 职位对应的权力表
        std::map<uint32, uint32> _title_power;

    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_guild_shard_config = KFGuildShardConfig::Instance();
}

#endif