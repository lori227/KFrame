#include "KFConditionModule.hpp"

namespace KFrame
{
    bool KFConditionModule::CheckCondition( KFEntity* kfentity, const KFConditions* kfconditions )
    {
        if ( !kfconditions->IsValid() )
        {
            return false;
        }

        if ( kfconditions->IsEmpty() )
        {
            return true;
        }

        auto linkcount = kfconditions->_link_type.size();
        auto conditioncount = kfconditions->_condition_list.size();

        // 取出第一个
        auto conditionindex = 0u;
        auto result = CheckCondition( kfentity, kfconditions->_condition_list[ conditionindex++ ] );
        for ( auto i = 0u; i < conditioncount && i < linkcount; ++i )
        {
            auto linktype = kfconditions->_link_type[ i ];
            auto ok = CheckCondition( kfentity, kfconditions->_condition_list[ conditionindex + i ] );
            switch ( linktype )
            {
            case KFEnum::And:
                result &= ok;
                break;
            case KFEnum::ABit:
                result |= ok;
                break;
            }
        }

        return result;
    }

    bool KFConditionModule::CheckCondition( KFEntity* kfentity, const KFCondition* kfcondition )
    {
        // 计算左值
        auto leftvalue = CalcExpression( kfentity, kfcondition->_left );

        // 计算右值
        auto rightvalue = CalcExpression( kfentity, kfcondition->_right );

        // 判断左有值
        return KFUtility::CheckOperate( leftvalue, kfcondition->_check_type, rightvalue );
    }

    uint32 KFConditionModule::CalcExpression( KFEntity* kfentity, const KFExpression* kfexpression )
    {
        if ( kfexpression->_data_list.empty() )
        {
            return 0u;
        }

        auto datacount = kfexpression->_data_list.size();
        auto operatorcount = kfexpression->_operator_list.size();

        // 取出第一个
        auto dataindex = 0u;
        auto result = CalcConditionData( kfentity, kfexpression->_data_list[ dataindex++ ] );
        for ( auto i = 0u; i < datacount && i < operatorcount; ++i )
        {
            auto operatortype = kfexpression->_operator_list[ i ];
            auto value = CalcConditionData( kfentity, kfexpression->_data_list[ dataindex + i ] );
            result = KFUtility::Operate( operatortype, result, value );
        }

        return result;
    }

    uint32 KFConditionModule::CalcConditionData( KFEntity* kfentity, const KFConditionData* kfconditiondata )
    {
        auto result = 0u;
        if ( kfconditiondata->IsConstant() )
        {
            auto kfconstant = ( const KFConditionConstant* )kfconditiondata;
            result = kfconstant->_value;
        }
        else if ( kfconditiondata->IsVariable() )
        {
            auto kfvariable = ( const KFConditionVariable* )kfconditiondata;
            if ( !kfvariable->_parent_name.empty() )
            {
                result = kfentity->Get<uint32>( kfvariable->_parent_name, kfvariable->_data_name );
            }
            else if ( kfvariable->_data_id == 0u )
            {
                result = kfentity->Get<uint32>( kfvariable->_data_name );
            }
            else
            {
                result = kfentity->GetConfigValue( kfvariable->_data_name, kfvariable->_data_id );
            }
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFConditionModule::AddCondition( KFData* kfconditiondata, const VectorUInt32& conditionlist )
    {
        auto kfconditionrecord = kfconditiondata->Find( __KF_STRING__( condition ) );
        for ( auto conditionid : conditionlist )
        {
            auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            auto kfcondition = _kf_kernel->CreateObject( kfconditionrecord->_data_setting );
            kfcondition->Set( __KF_STRING__( id ), conditionid );
            kfconditionrecord->Add( conditionid, kfcondition );
        }
    }

    bool KFConditionModule::CheckConditionLimit( KFEntity* kfentity, KFData* kfdata, const KFConditionSetting* kfsetting )
    {
        if ( kfsetting->_limits.empty() )
        {
            return true;
        }

        for ( auto& limit : kfsetting->_limits )
        {
            auto kfchild = kfdata->Find( limit._data_name );
            if ( kfchild == nullptr )
            {
                continue;
            }

            auto value = kfchild->Get();
            auto ok = KFUtility::CheckOperate( value, limit._operate, limit._data_value );
            if ( !ok )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __CHECK_CONDITION__( checkfunciton )\
    bool complete = false;\
    auto kfconditionrecord = kfconditiondata->Find( __KF_STRING__( condition ) );\
    std::list< KFData* > conditionlist;\
    for ( auto kfcondition = kfconditionrecord->First(); kfcondition != nullptr; kfcondition = kfconditionrecord->Next() )\
    {\
        conditionlist.push_back( kfcondition );\
    }\
    if ( conditionlist.empty() )\
    {\
        return true;\
    }\
    switch ( conditiontype )\
    {\
    case KFEnum::ABit:	\
    {\
        complete = false;\
        for ( auto kfcondition : conditionlist )\
        {\
            auto result = checkfunciton;\
            if ( result == KFConditionEnum::UpdateDone )\
            {\
                complete = true;\
                break;\
            }\
        }\
    }\
    break;\
    case KFEnum::And:	\
    {\
        complete = true;\
        for ( auto kfcondition : conditionlist )\
        {\
            auto result = checkfunciton;\
            if ( result != KFConditionEnum::UpdateDone )\
            {\
                complete = false;\
            }\
        }\
    }\
    break;\
    default:\
        break;\
    }\
    return complete;\

    bool KFConditionModule::InitCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, bool update )
    {
        __CHECK_CONDITION__( InitCondition( kfentity, kfcondition, limitmask, update ) );
    }

    uint32 KFConditionModule::InitCondition( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, bool update )
    {
        auto conditionid = kfcondition->Get( kfcondition->_data_setting->_config_key_name );
        auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR__( "condition=[{}] can't find setting!", conditionid );
            return KFConditionEnum::UpdateFailed;
        }

        if ( kfsetting->_condition_define == nullptr ||
                kfsetting->_condition_define->_init_calc_type == KFConditionEnum::InitNull )
        {
            return KFConditionEnum::UpdateFailed;
        }

        // 地点限制
        if ( KFUtility::HaveBitMask<uint32>( kfsetting->_limit_mask, KFConditionEnum::LimitPlace ) )
        {
            if ( KFUtility::HaveBitMask<uint32>( limitmask, KFConditionEnum::LimitPlace ) )
            {
                return KFConditionEnum::UpdateFailed;
            }
        }

        auto conditionvalue = 0u;
        switch ( kfsetting->_condition_define->_init_calc_type )
        {
        case KFConditionEnum::InitCalcCount:
            conditionvalue = InitCalcRecordCount( kfentity, kfsetting );
            break;
        case KFConditionEnum::InitCheckExist:
            conditionvalue = InitCalcRecordExist( kfentity, kfsetting );
            break;
        case KFConditionEnum::InitGetValue:
            conditionvalue = InitCalcGetValue( kfentity, kfsetting );
            break;
        default:
            break;
        }

        if ( !update )
        {
            kfcondition->Set<uint32>( kfcondition->_data_setting->_value_key_name, conditionvalue );
        }
        else
        {
            kfentity->UpdateData( kfcondition, kfcondition->_data_setting->_value_key_name, KFEnum::Set, conditionvalue );
        }

        auto ok = KFUtility::CheckOperate( conditionvalue, kfsetting->_done_type, kfsetting->_done_value );
        if ( ok )
        {
            return KFConditionEnum::UpdateDone;
        }

        return KFConditionEnum::UpdateOk;
    }

    uint32 KFConditionModule::InitCalcRecordCount( KFEntity* kfentity, const KFConditionSetting* kfsetting )
    {
        auto kfrecord = kfentity->Find( kfsetting->_condition_define->_parent_name );
        if ( kfrecord == nullptr )
        {
            return 0u;
        }

        auto recordcount = 0u;
        if ( kfsetting->_limits.empty() )
        {
            recordcount = kfrecord->Size();
        }
        else
        {
            for ( auto kfchild = kfrecord->First(); kfchild != nullptr; kfchild = kfrecord->Next() )
            {
                auto ok = CheckConditionLimit( kfentity, kfchild, kfsetting );
                if ( ok )
                {
                    ++recordcount;
                }
            }
        }

        return recordcount;
    }

    uint32 KFConditionModule::InitCalcRecordExist( KFEntity* kfentity, const KFConditionSetting* kfsetting )
    {
        auto kfrecord = kfentity->Find( kfsetting->_condition_define->_parent_name );
        if ( kfrecord == nullptr )
        {
            return 0u;
        }

        auto keyid = kfsetting->GetLimitValue( __KF_STRING__( id ) );
        auto kfdata = kfrecord->Find( keyid );
        if ( kfdata == nullptr )
        {
            return 0u;
        }

        return 1u;
    }

    uint32 KFConditionModule::InitCalcGetValue( KFEntity* kfentity, const KFConditionSetting* kfsetting )
    {
        if ( kfsetting->_condition_define->_parent_name == __KF_STRING__( player ) )
        {
            return kfentity->Get<uint32>( kfsetting->_condition_define->_data_name );
        }

        // 集合
        auto kfrecord = kfentity->Find( kfsetting->_condition_define->_parent_name );
        if ( kfrecord == nullptr )
        {
            return 0u;
        }

        for ( auto kfchild = kfrecord->First(); kfchild != nullptr; kfchild = kfrecord->Next() )
        {
            auto ok = CheckConditionLimit( kfentity, kfchild, kfsetting );
            if ( ok )
            {
                return kfchild->Get<uint32>( kfsetting->_condition_define->_data_name );
            }
        }

        return 0u;
    }

    bool KFConditionModule::UpdateCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 conditionid, uint32 operate, uint32 conditionvalue )
    {
        __CHECK_CONDITION__( UpdateCondition( kfentity, kfcondition, conditionid, operate, conditionvalue ) );
    }

    bool KFConditionModule::UpdateAddCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata )
    {
        __CHECK_CONDITION__( UpdateAddCondition( kfentity, kfcondition, limitmask, kfdata ) );
    }

    bool KFConditionModule::UpdateRemoveCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata )
    {
        __CHECK_CONDITION__( UpdateRemoveCondition( kfentity, kfcondition, limitmask, kfdata ) );
    }

    bool KFConditionModule::UpdateUpdateCondition( KFEntity* kfentity, KFData* kfconditiondata, uint32 conditiontype, uint32 limitmask, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        __CHECK_CONDITION__( UpdateUpdateCondition( kfentity, kfcondition, limitmask, kfdata, operate, value, nowvalue ) );
    }

#define __UPDATE_CONDITON__( updatefunction, limitdata )\
    auto conditionid = kfcondition->Get( kfcondition->_data_setting->_config_key_name );\
    auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );\
    if ( kfsetting == nullptr )\
    {\
        __LOG_ERROR__( "condition=[{}] can't find setting!", conditionid );\
        return KFConditionEnum::UpdateFailed;\
    }\
    if ( kfsetting->_condition_define == nullptr )\
    {\
        return KFConditionEnum::UpdateFailed;\
    }\
    auto operatetype = 0u;\
    auto operatevalue = 0u;\
    std::tie( operatetype, operatevalue ) = updatefunction;\
    if ( operatetype == 0u || operatevalue == 0u )\
    {\
        return KFConditionEnum::UpdateFailed;\
    }\
    if ( !KFUtility::HaveBitMask<uint32>( kfsetting->_limit_mask, KFConditionEnum::LimitStatus ) )\
    {\
        auto conditionvalue = kfcondition->Get<uint32>( kfcondition->_data_setting->_value_key_name );\
        if ( KFUtility::CheckOperate( conditionvalue, kfsetting->_done_type, kfsetting->_done_value ) )\
        {\
            return KFConditionEnum::UpdateDone;\
        }\
    }\
    if ( KFUtility::HaveBitMask<uint32>( kfsetting->_limit_mask, KFConditionEnum::LimitPlace ) )\
    {\
        if ( KFUtility::HaveBitMask<uint32>( limitmask, KFConditionEnum::LimitPlace ) )\
        {\
            return KFConditionEnum::UpdateFailed;\
        }\
    }\
    if ( !CheckConditionLimit( kfentity, limitdata, kfsetting ) )\
    {\
        return KFConditionEnum::UpdateFailed;\
    }\
    auto conditionvalue = kfentity->UpdateData( kfcondition, kfcondition->_data_setting->_value_key_name, operatetype, operatevalue );\
    auto ok = KFUtility::CheckOperate<uint32>( conditionvalue, kfsetting->_done_type, kfsetting->_done_value );\
    if ( ok )\
    {\
        return KFConditionEnum::UpdateDone;\
    }\
    return KFConditionEnum::UpdateOk;\

    uint32 KFConditionModule::UpdateAddCondition( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata )
    {
        __UPDATE_CONDITON__( CalcAddConditionValue( kfentity, kfsetting, kfdata ), kfdata );
    }

    uint32 KFConditionModule::UpdateRemoveCondition( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata )
    {
        __UPDATE_CONDITON__( CalcRemoveConditionValue( kfentity, kfsetting, kfdata ), kfdata );
    }

    uint32 KFConditionModule::UpdateUpdateCondition( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        __UPDATE_CONDITON__( CalcUpdateConditionValue( kfentity, kfsetting, kfdata, operate, value, nowvalue ), kfdata->GetParent() );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcAddConditionValue( KFEntity* kfentity, const KFConditionSetting* kfsetting, KFData* kfdata )
    {
        if ( kfdata->_data_setting->_name != kfsetting->_condition_define->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_condition_define->_trigger_list )
        {
            if ( trigger._call_type != KFConditionEnum::AddCall )
            {
                continue;
            }

            auto value = 1u;
            if ( !kfsetting->_condition_define->_data_name.empty() )
            {
                value = kfdata->Get( kfsetting->_condition_define->_data_name );
            }

            auto operatevalue = trigger.CalcUpdateValue( KFEnum::Set, value, value );
            if ( operatevalue != 0u )
            {
                return std::make_tuple( trigger._use_operate, operatevalue );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcRemoveConditionValue( KFEntity* kfentity, const KFConditionSetting* kfsetting, KFData* kfdata )
    {
        if ( kfdata->_data_setting->_name != kfsetting->_condition_define->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_condition_define->_trigger_list )
        {
            if ( trigger._call_type != KFConditionEnum::RemoveCall )
            {
                continue;
            }

            auto value = 1u;
            if ( !kfsetting->_condition_define->_data_name.empty() )
            {
                value = kfdata->Get( kfsetting->_condition_define->_data_name );
            }

            auto operatevalue = trigger.CalcUpdateValue( KFEnum::Dec, value, 0 );
            if ( operatevalue != 0u )
            {
                return std::make_tuple( trigger._use_operate, operatevalue );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcUpdateConditionValue( KFEntity* kfentity, const KFConditionSetting* kfsetting, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        if ( kfdata->_data_setting->_name != kfsetting->_condition_define->_data_name ||
                kfdata->GetParent()->_data_setting->_name != kfsetting->_condition_define->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_condition_define->_trigger_list )
        {
            if ( trigger._call_type != KFConditionEnum::UpdateCall )
            {
                continue;
            }

            auto operatevalue = trigger.CalcUpdateValue( operate, value, nowvalue );
            if ( operatevalue != 0u )
            {
                return std::make_tuple( trigger._use_operate, operatevalue );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    uint32 KFConditionModule::UpdateCondition( KFEntity* kfentity, KFData* kfcondition, uint32 conditionid, uint32 operate, uint32 conditionvalue )
    {
        auto updateconditionid = kfcondition->GetKeyID();
        auto kfsetting = KFConditionConfig::Instance()->FindSetting( updateconditionid );
        if ( kfsetting == nullptr || kfsetting->_condition_define == nullptr )
        {
            return KFConditionEnum::UpdateFailed;
        }

        if ( updateconditionid == conditionid )
        {
            conditionvalue = kfentity->UpdateData( kfcondition, kfcondition->_data_setting->_value_key_name, operate, conditionvalue );
        }
        else
        {
            conditionvalue = kfcondition->Get<uint32>( kfcondition->_data_setting->_value_key_name );
        }

        auto ok = KFUtility::CheckOperate<uint32>( conditionvalue, kfsetting->_done_type, kfsetting->_done_value );
        if ( ok )
        {
            return KFConditionEnum::UpdateDone;
        }

        return KFConditionEnum::UpdateOk;
    }
}

