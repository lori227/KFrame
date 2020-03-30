#ifndef __KF_DROP_GROUP_CONFIG_H__
#define __KF_DROP_GROUP_CONFIG_H__

#include "KFCore/KFCondition.h"
#include "KFZConfig/KFSetting.h"
#include "KFZConfig/KFConfig.h"
#include "KFDropDataConfig.hpp"

namespace KFrame
{
    class KFDropGroupWeight : public KFWeight
    {
    public:
        // 是否要清除变量
        bool _is_clear_var = false;

        // 掉落数据id
        uint32 _drop_data_id = 0u;

        // 条件
        KFConditions _conditions;

        // 掉落数据配置
        const KFDropDataSetting* _drop_data_setting = nullptr;
    };

    class KFDropSetting : public KFIntSetting
    {
    public:
        enum MyEnum
        {
            MutexCondition = 1,		// 互斥条件
            OverlayCondition = 2,	// 叠加条件
        };

    public:
        // 是否纪录掉落次数
        bool _is_drop_count = false;

        // 是否排除相同的
        bool _is_exclude = false;

        // 随机类型
        uint32 _rand_type = 0u;

        // 条件类型
        uint32 _condition_type = 0u;

        // 必掉的数据
        KFVector< KFDropGroupWeight > _necessary_list;

        // 掉落数据
        KFWeightList< KFDropGroupWeight > _rand_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    class KFDropGroupConfig : public KFConfigT< KFDropSetting >, public KFInstance< KFDropGroupConfig >
    {
    public:
        KFDropGroupConfig()
        {
            _file_name = "dropgroup";
        }

        // 全部读取
        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFDropSetting* kfsetting );

        // 初始化掉落
        void InitDropDataSetting( KFDropSetting* kfsetting, KFDropGroupWeight* dropgroupweight );

        // 初始化掉落条件
        void InitDropConditonSetting( KFDropSetting* kfsetting, KFExpression* kfexpression );
    };
}

#endif