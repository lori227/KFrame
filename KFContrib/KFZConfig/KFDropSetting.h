#ifndef __KF_DROP_SETTING_H__
#define __KF_DROP_SETTING_H__

#include "KFCore/KFElement.h"
#include "KFCore/KFCondition.h"
#include "KFZConfig/KFSetting.h"

namespace KFrame
{
    class KFDropData
    {
    public:
        // 掉落属性名
        std::string _data_name;

        // 掉落key
        uint32 _data_key = 0u;

        // 掉落数值
        std::string _data_value;

        // 掉落数值
        uint32 _min_value = 0u;
        uint32 _max_value = 0u;

        // 掉落数据
        KFElements _elements;

    public:
        // 获得数值
        uint32 GetValue() const
        {
            return KFGlobal::Instance()->RandRange( _min_value, _max_value, 1 );
        }
    };

    class KFDropWeight : public KFWeight
    {
    public:
        // 掉落权重
        KFDropData _drop_data;

        // 是否要清除变量
        bool _is_clear_var = false;

        // 条件
        KFConditions _conditions;
    };

    class KFDropSetting : public KFIntSetting
    {
    public:
        // 变量id
        uint32 _var_id = 0;

        // 必掉的数据
        KFVector< KFDropWeight > _necessary_list;

        // 掉落数据
        KFWeightList< KFDropWeight > _rand_list;
    };
}

#endif