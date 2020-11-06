#include "KFrame.h"
#include "KFCondition.h"
#include "KFAnalysis.hpp"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////
    KFExpression::~KFExpression()
    {
        for ( auto kfdata : _data_list )
        {
            __KF_DELETE__( KFConditionAbstract, kfdata );
        }
        _data_list.clear();
        _operator_list.clear();
    }

    KFCondition::~KFCondition()
    {
        if ( _left != nullptr )
        {
            __KF_DELETE__( KFExpression, _left );
            _left = nullptr;
        }

        if ( _right != nullptr )
        {
            __KF_DELETE__( KFExpression, _right );
            _right = nullptr;
        }
    }

    KFConditions::~KFConditions()
    {
        Reset();
    }
    ///////////////////////////////////////////////////////////////////////////////////
    bool KFConditions::IsEmpty() const
    {
        return _condition_list.empty();
    }

    bool KFConditions::IsValid() const
    {
        return _is_valid;
    }

    void KFConditions::Reset()
    {
        for ( auto kfcondition : _condition_list )
        {
            __KF_DELETE__( KFCondition, kfcondition );
        }
        _condition_list.clear();
        _link_type.clear();
    }

    bool KFConditions::Parse( std::string& strcondition, const char* function, uint32 line )
    {
        Reset();
        _is_valid = true;
        if ( strcondition.empty() )
        {
            return true;
        }

        KFAnalysis::Cleanup( strcondition );
        _str_condition = strcondition;

        _is_valid = KFAnalysis::Parse( this, strcondition );
        if ( !_is_valid )
        {
            Reset();
            __LOG_ERROR_FUNCTION__( function, line, "condition=[{}] parse failed", strcondition );
        }

        return _is_valid;
    }
}