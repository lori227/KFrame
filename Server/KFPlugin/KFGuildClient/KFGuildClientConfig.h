#ifndef __KF_GUILD_CLIENT_CONFIG_H__
#define __KF_GUILD_CLIENT_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFAgent.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    namespace KFGuildEnum
    {
        enum EConstDefine
        {
            InitLevel = 1,
            MaxLevel = 9,

            MaxNameLength = 10,

            MaxManifesto = 50,
        };
    }
    class KFGuildSetting
    {
    public:
        KFGuildSetting()
        {
            _level = KFGuildEnum::InitLevel;
        }
        // 帮派等级
        uint32 _level;

        // 升级消费
        KFAgents _cost_item;

        // 最大成员数量
        uint32 _max_member;

        // 升级所需活跃度
        uint32 _upgrade_activeness;

        // 降级所需活跃度
        uint32 _degrade_activeness;

        // 帮派日志条数上限
        uint32 _max_log;

        // 帮派勋章列表
        std::string _medal_list;

        bool IsValidMedal( uint32 medal ) const;

    };

    class KFGuildConfig : public KFConfig, public KFSingleton< KFGuildConfig >
    {
    public:
        KFGuildConfig();
        ~KFGuildConfig();

        //加载配置
        bool LoadConfig();

        // 查找帮派配置
        const KFGuildSetting* FindGuildSetting( uint32 level ) const;

    public:
        KFMap<uint32, uint32, KFGuildSetting > _guild_setting;
    };

    //////////////////////////////////////////////////////////////////////////
    static auto _kf_guild_config = KFGuildConfig::Instance();
    //////////////////////////////////////////////////////////////////////////
}


#endif//__KF_GUILD_CLIENT_CONFIG_H__

