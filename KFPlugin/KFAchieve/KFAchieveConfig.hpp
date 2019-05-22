#ifndef __KF_ACHIEVE_CONFIG_H__
#define __KF_ACHIEVE_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFAchieveSetting
    {
    public:
        // 判断是否可以更新
        bool CheckCanUpdate( uint64 key, uint64 operate ) const;

        // 判断触发值
        uint64 CheckTriggerValue( uint64 operatevalue, uint64 nowvalue ) const;

    public:
        uint32 _id = 0;					// id
        std::string _parent_name;		// 类型
        std::string _data_name;			// 属性
        uint32 _data_key = 0;			// 键值

        uint32 _trigger_type = 0;		// 触发条件
        uint32 _trigger_value = 0;		// 触发值

        uint32 _use_type = 0;			// 使用的类型
        uint32 _use_value = 0;			// 使用的实际数值

        uint32 _operate = 0;			// 操作类型

        uint32 _done_value = 0;			// 完成数值
        uint32 _done_type = 0;			// 完成条件

        KFElements _limits;				// 限制条件
        KFElements _rewards;			// 奖励
    };

    class KFAchieveType
    {
    public:
        inline void AddKFAchieveType( KFAchieveSetting* setting )
        {
            _achieve_list.push_back( setting );
        }

    public:
        // 类型列表
        std::vector< KFAchieveSetting* > _achieve_list;
    };

    class KFAchieveTypes
    {
    public:
        void AddAchieveType( KFAchieveSetting* kfsetting );
        const KFAchieveType* FindAchieveType( const std::string& dataname ) const;

    public:
        std::unordered_map< std::string, KFAchieveType > _achieve_type;
    };

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    class KFAchieveConfig : public KFConfig, public KFSingleton< KFAchieveConfig >
    {
    public:
        KFAchieveConfig()
        {
            _file = "achieve.config";
        }

        // 读取配置
        bool LoadConfig();

        // 获得成就配置
        const KFAchieveSetting* FindAchieveSetting( uint32 id ) const;

        // 获得某种类型的所有成就
        const KFAchieveType* FindTypeAchieve( const std::string& parentname, const std::string& dataname ) const;
    protected:
        // 添加成就类型
        void AddAchieveType( KFAchieveSetting* kfsetting );

    private:
        // 成就列表
        KFHashMap< uint32, uint32, KFAchieveSetting > _achieve_setting;

        // 类型列表
        std::unordered_map< std::string, KFAchieveTypes > _achieve_types;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_achieve_config = KFAchieveConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif