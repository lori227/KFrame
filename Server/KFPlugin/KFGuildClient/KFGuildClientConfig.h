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
            InviteGuildMailId = 6,

            MaxNameLength = 10,

            MaxManifesto = 50,
        };
        enum EnumTitlePowerEnum
        {
            Mask_Invite_Data = 1 << 1,		// 邀请权力
            Mask_Review_Data = 1 << 2,		// 审批权力
            Mask_Dissolve_Data = 1 << 3,	// 解散帮派权力
            Mask_Dismissal_Data = 1 << 4,	// 解雇成员(只能解雇职位比这身低的)
            Mask_Appoint_Data = 1 << 5,		// 任命权力(只能任命职位比这身低的)
            Mask_Modify_Data = 1 << 6,		// 修改帮派徽章等
            Mask_Upgrade_Data = 1 << 7,		// 升级帮派
            Mask_Manger_Data = 1 << 8,		// 帮派管理(审批按钮等)
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

        // 最大日志
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

        // 是否拥有某项权力
        bool IsOwnPower( uint32 title, uint32 datamask ) const;

        // 职位是否是有效
        bool IsValidTitle( uint32 title ) const;
        // 获取申请列表/邀请列表有效时间
        inline uint32 GetKeepTime()
        {
            return _keep_time;
        }

        // 设置帮派管理标志是否合法
        bool IsVaildSwitch( const std::string& type, uint32 flag );

    public:
        KFMap<uint32, uint32, KFGuildSetting > _guild_setting;

        uint32 _max_level;

        // 职位对应的权力表
        std::map<uint32, uint32> _title_power;

        // 申请列表邀请列表有效时间
        uint32 _keep_time;

        // 帮派审批开关
        std::string _review_switch;

        // 帮派申请等级要求
        std::string _req_level;

        // 帮派日志单页最大条数
        uint32 _max_guild_log_page;

    };

    //////////////////////////////////////////////////////////////////////////
    static auto _kf_guild_config = KFGuildConfig::Instance();
    //////////////////////////////////////////////////////////////////////////
}


#endif//__KF_GUILD_CLIENT_CONFIG_H__

