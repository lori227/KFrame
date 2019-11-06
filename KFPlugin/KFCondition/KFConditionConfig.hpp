#ifndef __KF_CONDITION_CONFIG_H__
#define __KF_CONDITION_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class KFConditionTrigger
    {
    public:
        // 回调类型
        uint32 _call_type = 0u;

        // 触发条件
        uint32 _trigger_type = 0;

        // 触发判断
        uint32 _trigger_check = 0;

        // 触发值
        uint32 _trigger_value = 0;

        // 操作类型
        uint32 _use_operate = 0;

        // 使用的类型
        uint32 _use_type = 0;

        // 使用的实际数值
        uint32 _use_value = 0;

    public:
        // 计算更新数值
        uint64 CalcUpdateValue( uint64 operate, uint64 operatevalue, uint64 nowvalue ) const;
    };

    class KFConditionDefine: public KFStrSetting
    {
    public:
        // 父属性
        std::string _parent_name;

        // 属性
        std::string _data_name;

        // 初始值
        uint32 _init_calc_type;

        // 触发列表
        std::vector< KFConditionTrigger > _trigger_list;
    };

    class KFConditionDefineConfig : public KFConfigT< KFConditionDefine >, public KFInstance< KFConditionDefineConfig >
    {
    public:
        KFConditionDefineConfig()
        {
            _file_name = "conditiondefine";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFConditionDefine* kfsetting );
    };
    /////////////////////////////////////////////////////////////////////////////////
    class KFConditionLimit
    {
    public:
        std::string _data_name;
        uint64 _data_value;
        uint32 _operate;
    };

    class KFConditionSetting : public KFIntSetting
    {
    public:
        // 文件名
        std::string _file_name;

        // 条件定义
        std::string _str_condition;
        const KFConditionDefine* _condition_define = nullptr;

        // 限制条件
        std::vector< KFConditionLimit > _limits;

        // 完成条件
        uint32 _done_type = 0;

        // 完成数值
        uint32 _done_value = 0;

        // 限制类型
        uint32 _limit_mask = 0u;

    public:

        // 获得id属性
        uint64 GetLimitValue( const std::string& name ) const;
    };

    class KFConditionConfig : public KFConfigT< KFConditionSetting >, public KFInstance< KFConditionConfig >
    {
    public:
        KFConditionConfig()
        {
            _file_name = "condition";
        }

        // 加载完成
        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFConditionSetting* kfsetting );

        // 解析限制条件
        bool AnalysisLimit( KFConditionSetting* kfsetting, const std::string& strlimit );
    };
}

#endif