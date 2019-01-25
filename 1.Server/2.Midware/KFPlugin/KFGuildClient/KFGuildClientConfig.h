#ifndef __KF_GUILD_CLIENT_CONFIG_H__
#define __KF_GUILD_CLIENT_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
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

        enum  EGuildTriggerEnum
        {
            Finish = 1,
            Amass = 2,
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
        KFElements _cost_item;

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


    // 帮派成员活跃度配置
    class KFGuildActivenessSetting
    {
    public:
        uint32 _id;							// 任务id
        uint32 _add_activeness_value;		// 单次任务增加活跃度值
        std::string _parent_name;			// 类型
        std::string _data_name;				// 属性
        uint32 _operate;					// 操作类型
        uint32 _day_limit;					// 单日限制获取活跃度上限
        uint32 _trigger_type;				// 触发条件 1:单次触发(=) 2:累加触发(+)
        uint32 _done_value;					// 完成数值
        uint32 _key;						// 键值

        // 判断是否可以更新
        bool CheckCanUpdate( uint32 key, uint32 operate ) const;

        // 获取增加的活跃度值
        uint32 GetAddActivenessValue( uint32 value, uint32 ownactiveness, uint32& usevalue );
    };

    class KFGuildActivenessType
    {
    public:
        inline void AddKFGuildActivenessType( KFGuildActivenessSetting* setting )
        {
            _guild_activeness_type.push_back( setting );
        }

    public:
        // 类型列表
        std::vector< KFGuildActivenessSetting* > _guild_activeness_type;
    };

    class KFGuildActivenessTypes
    {
    public:
        void AddKFGuildActivenessType( KFGuildActivenessSetting* setting );
        const KFGuildActivenessType* FindKFGuildActivenessType( const std::string& dataname ) const;

    public:
        std::map< std::string, KFGuildActivenessType > _guild_activenesstype;
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

        // 获取最大成员周活跃上限
        inline uint32 GetMaxWeekActiveness()
        {
            return _max_week_activeness;
        }

        // 设置帮派管理标志是否合法
        bool IsVaildSwitch( const std::string& type, uint32 flag );

        // 获得某种类型的所有成就
        const KFGuildActivenessType* FindTypeActiveness( const std::string& parentname, const std::string& dataname ) const;

    protected:
        void AddKFActivenessSetting( KFGuildActivenessSetting* kfsetting );

        void AddKFTypeActiveness( KFGuildActivenessSetting* kfsetting );

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

        // 帮派成员周活跃度上限
        uint32 _max_week_activeness;
    private:
        // 成就列表
        KFMap< uint32, uint32, KFGuildActivenessSetting > _guild_activeness_setting;
        // 类型列表
        std::map< std::string, KFGuildActivenessTypes > _object_types;
    };



    //////////////////////////////////////////////////////////////////////////
    static auto _kf_guild_config = KFGuildConfig::Instance();
    //////////////////////////////////////////////////////////////////////////
}


#endif//__KF_GUILD_CLIENT_CONFIG_H__

