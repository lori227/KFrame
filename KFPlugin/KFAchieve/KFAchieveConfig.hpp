#ifndef __KF_ACHIEVE_CONFIG_H__
#define __KF_ACHIEVE_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    class KFAchieveSetting
    {
    public:
        uint32 _id;					// id
        uint32 _level;				// 等级限制

        std::string _parent_name;	// 类型
        std::string _data_name;		// 属性

        uint32 _trigger_type;		// 触发条件
        uint32 _trigger_value;		// 触发值

        uint32 _use_type;			// 使用的类型
        uint32 _use_value;			// 使用的实际数值

        uint32 _key;				// 键值
        uint32 _operate;			// 操作类型

        uint32 _done_value;			// 完成数值
        uint32 _done_type;			// 完成条件

        KFElements _rewards;		// 奖励

        // 判断是否可以更新
        bool CheckCanUpdate( uint32 key, uint32 level, uint32 operate ) const;
        uint32 CheckTriggerValue( uint32 operatevalue, uint32 nowvalue ) const;
        uint32 GetUseValue( uint32 operatevalue ) const;
    };

    class KFAchieveType
    {
    public:
        inline void AddKFAchieveType( KFAchieveSetting* setting )
        {
            _achieve_type.push_back( setting );
        }

    public:
        // 类型列表
        std::vector< KFAchieveSetting* > _achieve_type;
    };

    // use std::tuple
    class KFDataAchieveTypes
    {
    public:
        void AddKFAchieveType( KFAchieveSetting* setting );
        const KFAchieveType* FindKFAchieveType( const std::string& dataname ) const;

    public:
        std::unordered_map< std::string, KFAchieveType > _achievetype;
    };

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    class KFAchieveConfig : public KFConfig, public KFSingleton< KFAchieveConfig >
    {
    public:
        // 获得成就配置
        const KFAchieveSetting* FindAchieveSetting( uint32 id ) const;

        // 获得某种类型的所有成就
        const KFAchieveType* FindTypeAchieve( const std::string& parentname, const std::string& dataname ) const;

        // 读取配置
        bool LoadConfig();

    protected:
        void AddKFTypeAchieve( KFAchieveSetting* kfsetting );

    private:
        // 成就列表
        KFHashMap< uint32, uint32, KFAchieveSetting > _achieve_setting;

        // 类型列表
        std::unordered_map< std::string, KFDataAchieveTypes > _object_types;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_achieve_config = KFAchieveConfig::Instance();
}

#endif