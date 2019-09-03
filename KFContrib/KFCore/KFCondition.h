#ifndef __KF_CONDITION_H__
#define __KF_CONDITION_H__

#include "KFInclude.h"

namespace KFrame
{
    // 条件数据
    class KFConditionData
    {
    public:
        virtual bool IsConstant() const
        {
            return false;
        }

        virtual bool IsVariable() const
        {
            return false;
        }
    };

    // 常量数据
    class KFConditionConstant : public KFConditionData
    {
    public:
        virtual bool IsConstant() const
        {
            return true;
        }
    public:
        uint32 _value = 0;
    };

    // 变量数据
    class KFConditionVariable : public KFConditionData
    {
    public:
        virtual bool IsVariable() const
        {
            return true;
        }

    public:
        // 父属性
        std::string _parent_name;

        // 子属性名
        std::string _data_name;

        // 属性配置ID
        uint32 _data_id = 0u;
    };

    // 表达式
    class KFExpression
    {
    public:
        ~KFExpression();
    public:
        // 数据列表
        std::vector< KFConditionData* > _data_list;

        // 计算符号列表
        std::vector< uint32 > _operator_list;
    };

    // 数据判断条件
    class KFCondition
    {
    public:
        ~KFCondition();
    public:
        // 左表达式
        KFExpression* _left = nullptr;

        // 右表达式
        KFExpression* _right = nullptr;

        // 判断类型
        uint32 _check_type = 0u;
    };

    // 数据判断条件
    class KFConditions
    {
    public:
        ~KFConditions();

        // 是否为空
        bool IsEmpty() const;

        // 是否有效
        bool IsValid() const;

        // 格式化
        bool Parse( std::string& strcondition, uint64 id, const char* function, uint32 line );

    protected:
        // 重置
        void Reset();
    public:
        // 原始字串
        std::string _str_condition;

        // 是否有效
        bool _is_valid = true;

        // 条件列表
        std::vector< KFCondition* > _condition_list;

        // 连接列表
        std::vector<uint32> _link_type;
    };
}


#endif
