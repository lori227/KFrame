#include "KFrame.h"
#include "KFStaticCondition.h"
#include "KFStaticAnalysis.hpp"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////
    KFStaticConditionExpression::~KFStaticConditionExpression()
    {
        _data_list.clear();
        _operator_list.clear();
    }

    KFStaticCondition::~KFStaticCondition()
    {
        _left = nullptr;
        _right = nullptr;
    }

    KFStaticConditions::~KFStaticConditions()
    {
        _condition_list.clear();
        _link_type.clear();
    }
    ///////////////////////////////////////////////////////////////////////////////////
    bool KFStaticConditions::IsEmpty() const
    {
        return _condition_list.empty();
    }

    bool KFStaticConditions::IsValid() const
    {
        return _is_valid;
    }
}