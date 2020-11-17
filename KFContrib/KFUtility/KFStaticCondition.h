#ifndef __KF_CONDITION_H__
#define __KF_CONDITION_H__

#include "KFInclude.h"

namespace KFrame
{
    // 静态条件抽象
    class KFStaticConditionAbstract
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

    // 静态条件常量数据
    class KFStaticConditionConstant : public KFStaticConditionAbstract
    {
    public:
        virtual bool IsConstant() const
        {
            return true;
        }
    public:
        uint32 _value = 0;
    };

    // 静态条件变量数据
    class KFStaticConditionVariable : public KFStaticConditionAbstract
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

    // 静态条件表达式
    class KFStaticConditionExpression
    {
    public:
        ~KFStaticConditionExpression();
    public:
        // 数据列表
        std::vector< KFStaticConditionAbstract* > _data_list;

        // 计算符号列表
        UInt32Vector _operator_list;
    };

    // 静态条件
    class KFStaticCondition
    {
    public:
        ~KFStaticCondition();
    public:
        // 左表达式
        KFStaticConditionExpression* _left = nullptr;

        // 右表达式
        KFStaticConditionExpression* _right = nullptr;

        // 判断类型
        uint32 _check_type = 0u;
    };

    // 静态条件列表
    class KFStaticConditions
    {
    public:
        ~KFStaticConditions();

        // 是否为空
        bool IsEmpty() const;

        // 是否有效
        bool IsValid() const;

    public:
        // 原始字串
        std::string _str_condition;

        // 是否有效
        bool _is_valid = true;

        // 条件列表
        std::vector< KFStaticCondition* > _condition_list;

        // 连接列表
        UInt32Vector _link_type;
    };
    typedef std::shared_ptr< KFStaticConditions > StaticConditionsPtr;

    // 静态条件列表
    class KFStaticConditionList
    {
    public:
        // 判断类型
        uint32 _check_type = 0;

        // 静态条件列表
        std::vector< StaticConditionsPtr > _conditions_list;
    };
    typedef std::shared_ptr< KFStaticConditionList > StaticConditionListPtr;

}


#endif
