#include "KFrame.h"
#include "KFStaticCondition.h"
#include "KFStaticAnalysis.hpp"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////
    KFStaticConditionExpression::~KFStaticConditionExpression()
    {
        for ( auto kfdata : _data_list )
        {
            __KF_DELETE__( KFStaticConditionAbstract, kfdata );
        }
        _data_list.clear();
        _operator_list.clear();
    }

    KFStaticCondition::~KFStaticCondition()
    {
        if ( _left != nullptr )
        {
            __KF_DELETE__( KFStaticConditionExpression, _left );
            _left = nullptr;
        }

        if ( _right != nullptr )
        {
            __KF_DELETE__( KFStaticConditionExpression, _right );
            _right = nullptr;
        }
    }

    KFStaticConditions::~KFStaticConditions()
    {
        for ( auto kfcondition : _condition_list )
        {
            __KF_DELETE__( KFStaticCondition, kfcondition );
        }
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