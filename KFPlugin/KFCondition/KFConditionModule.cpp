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
            case KFEnum::Or:
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
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFConditionModule::CheckConditionLimit( KFEntity* kfentity, KFData* kfdata, const KFConditionSetting* kfsetting )
    {
        if ( kfsetting->_limits.empty() )
        {
            return true;
        }

        for ( auto& limits : kfsetting->_limits )
        {
            bool limitok = false;
            switch ( limits._link_type )
            {
            case KFEnum::Or:
            {
                for ( auto& limit : limits._limit )
                {
                    auto kfchild = kfdata->Find( limit._data_name );
                    if ( kfchild == nullptr )
                    {
                        continue;
                    }

                    auto value = kfchild->Get();
                    auto ok = KFUtility::CheckOperate( value, limit._operate, limit._data_value );
                    if ( ok )
                    {
                        limitok = true;
                        break;
                    }
                }
            }
            break;
            case KFEnum::And:
            {
                limitok = true;
                for ( auto& limit : limits._limit )
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
                        limitok = false;
                        break;
                    }
                }
            }
            break;
            }

            if ( !limitok )
            {
                return false;
            }
        }

        return true;
    }

    void KFConditionModule::AddCondition( KFEntity* kfentity, KFData* kfconditionobject, const UInt32Vector& conditionlist, uint32 conditiontype )
    {
        if ( conditionlist.empty() )
        {
            return;
        }

        auto kfconditionrecord = kfconditionobject->Find( __STRING__( condition ) );
        if ( kfconditionrecord == nullptr )
        {
            return;
        }

        kfconditionobject->Set( __STRING__( type ), conditiontype );
        for ( auto conditionid : conditionlist )
        {
            auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );
            if ( kfsetting == nullptr )
            {
                __LOG_ERROR__( "condition=[{}] can't find setting", conditionid );
                continue;
            }

            auto kfcondition = kfentity->CreateData( kfconditionrecord );

            // 前置条件
            AddCondition( kfentity, kfcondition, kfsetting->_limit_condition_list, kfsetting->_limit_condition_type );

            kfcondition->Set( __STRING__( id ), conditionid );
            kfconditionrecord->Add( conditionid, kfcondition );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __CHECK_CONDITION__( checkfunciton )\
    if ( kfconditionobject == nullptr )\
    {\
        return false;\
    }\
    auto conditiontype = kfconditionobject->Get<uint32>( __STRING__( type ) );\
    if ( conditiontype == 0u )\
    {\
        return true;\
    }\
    auto kfconditionrecord = kfconditionobject->Find( __STRING__( condition ) );\
    if ( kfconditionrecord == nullptr || kfconditionrecord->Size() == 0u )\
    {\
        return true;\
    }\
    bool complete = false;\
    switch ( conditiontype )\
    {\
    case KFEnum::Or:	\
    {\
        complete = false;\
        for ( auto kfcondition = kfconditionrecord->First(); kfcondition != nullptr; kfcondition = kfconditionrecord->Next() )\
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
        for ( auto kfcondition = kfconditionrecord->First(); kfcondition != nullptr; kfcondition = kfconditionrecord->Next() )\
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

    bool KFConditionModule::InitCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 limitmask, bool update )
    {
        __CHECK_CONDITION__( InitConditionData( kfentity, kfcondition, limitmask, update ) );
    }

    uint32 KFConditionModule::InitConditionData( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, bool update )
    {
        // 限制条件初始化
        {
            auto ok = InitCondition( kfentity, kfcondition, limitmask, update );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        auto conditionid = kfcondition->Get( kfcondition->_data_setting->_config_key_name );
        auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR__( "condition=[{}] can't find setting", conditionid );
            return KFConditionEnum::UpdateDone;
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
        case KFConditionEnum::InitGetValue:
            conditionvalue = InitCalcGetValue( kfentity, kfsetting );
            break;
        case KFConditionEnum::InitCheckExist:
            conditionvalue = InitCalcRecordExist( kfentity, kfsetting );
            break;
        case KFConditionEnum::InitCalcValue:
            conditionvalue = InitCalcRecordValue( kfentity, kfsetting );
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

    std::list< KFData* >& KFConditionModule::FindRecordList( KFEntity* kfentity, const std::string& name )
    {
        static std::list< KFData* > _recrods;
        _recrods.clear();

        auto kfrecord = kfentity->Find( name );
        if ( kfrecord != nullptr )
        {
            _recrods.push_back( kfrecord );
        }
        else
        {
            for ( auto kfchild = kfentity->First(); kfchild != nullptr; kfchild = kfentity->Next() )
            {
                if ( kfchild->_data_setting->_logic_name == name )
                {
                    _recrods.push_back( kfchild );
                }
            }
        }

        return _recrods;
    }

    uint32 KFConditionModule::InitCalcRecordCount( KFEntity* kfentity, const KFConditionSetting* kfsetting )
    {
        auto recordcount = 0u;
        auto& kfrecords = FindRecordList( kfentity, kfsetting->_condition_define->_parent_name );
        for ( auto kfrecord : kfrecords )
        {
            if ( kfsetting->_limits.empty() )
            {
                recordcount += kfrecord->Size();
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
        }

        return recordcount;
    }


    uint32 KFConditionModule::InitCalcRecordValue( KFEntity* kfentity, const KFConditionSetting* kfsetting )
    {
        auto recordvalue = 0u;
        auto& kfrecords = FindRecordList( kfentity, kfsetting->_condition_define->_parent_name );
        for ( auto kfrecord : kfrecords )
        {
            for ( auto kfchild = kfrecord->First(); kfchild != nullptr; kfchild = kfrecord->Next() )
            {
                auto ok = CheckConditionLimit( kfentity, kfchild, kfsetting );
                if ( ok )
                {
                    recordvalue += kfchild->Get<uint32>( kfchild->_data_setting->_value_key_name );
                }
            }
        }

        return recordvalue;
    }

    uint32 KFConditionModule::InitCalcRecordExist( KFEntity* kfentity, const KFConditionSetting* kfsetting )
    {
        auto& kfrecords = FindRecordList( kfentity, kfsetting->_condition_define->_parent_name );
        for ( auto kfrecord : kfrecords )
        {
            for ( auto& limits : kfsetting->_limits )
            {
                for ( auto& limit : limits._limit )
                {
                    if ( limit._data_name == __STRING__( id ) )
                    {
                        auto kfdata = kfrecord->Find( limit._data_value );
                        if ( kfdata != nullptr )
                        {
                            return 1u;
                        }
                    }
                }
            }
        }

        return 0u;
    }

    uint32 KFConditionModule::InitCalcGetValue( KFEntity* kfentity, const KFConditionSetting* kfsetting )
    {
        if ( kfsetting->_condition_define->_parent_name == __STRING__( player ) )
        {
            return kfentity->Get<uint32>( kfsetting->_condition_define->_data_name );
        }

        // 集合
        auto recordvalue = 0u;
        auto& kfrecords = FindRecordList( kfentity, kfsetting->_condition_define->_parent_name );
        for ( auto kfrecord : kfrecords )
        {
            for ( auto kfchild = kfrecord->First(); kfchild != nullptr; kfchild = kfrecord->Next() )
            {
                auto ok = CheckConditionLimit( kfentity, kfchild, kfsetting );
                if ( ok )
                {
                    recordvalue = kfchild->Get<uint32>( kfsetting->_condition_define->_data_name );
                    break;
                }
            }
        }

        return recordvalue;
    }

    uint32 KFConditionModule::CheckConditionData( KFEntity* kfentity, KFData* kfcondition )
    {
        auto conditionid = kfcondition->Get( kfcondition->_data_setting->_config_key_name );
        auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );
        if ( kfsetting == nullptr || kfsetting->_condition_define == nullptr )
        {
            return KFConditionEnum::UpdateFailed;
        }

        auto conditionvalue = kfcondition->Get<uint32>( kfcondition->_data_setting->_value_key_name );
        auto ok = KFUtility::CheckOperate<uint32>( conditionvalue, kfsetting->_done_type, kfsetting->_done_value );
        if ( ok )
        {
            return KFConditionEnum::UpdateDone;
        }

        return KFConditionEnum::UpdateOk;
    }

    bool KFConditionModule::CheckCondition( KFEntity* kfentity, KFData* kfconditionobject )
    {
        __CHECK_CONDITION__( CheckConditionData( kfentity, kfcondition ) );
    }

    bool KFConditionModule::UpdateCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 conditionid, uint32 operate, uint32 conditionvalue )
    {
        __CHECK_CONDITION__( UpdateConditionData( kfentity, kfcondition, conditionid, operate, conditionvalue ) );
    }

    bool KFConditionModule::UpdateAddCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 limitmask, KFData* kfdata )
    {
        __CHECK_CONDITION__( UpdateAddConditionData( kfentity, kfcondition, limitmask, kfdata ) );
    }

    bool KFConditionModule::UpdateRemoveCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 limitmask, KFData* kfdata )
    {
        __CHECK_CONDITION__( UpdateRemoveConditionData( kfentity, kfcondition, limitmask, kfdata ) );
    }

    bool KFConditionModule::UpdateUpdateCondition( KFEntity* kfentity, KFData* kfconditionobject, uint32 limitmask, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        __CHECK_CONDITION__( UpdateUpdateConditionData( kfentity, kfcondition, limitmask, kfdata, operate, value, nowvalue ) );
    }

#define __CLEAN_CONDITION__( cleanfunction )\
    auto conditionid = kfcondition->Get( kfcondition->_data_setting->_config_key_name );\
    auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );\
    if ( kfsetting == nullptr )\
    {\
        __LOG_ERROR__( "condition=[{}] can't find setting", conditionid );\
        return KFConditionEnum::UpdateDone;\
    }\
    auto operatetype = 0u;\
    auto operatevalue = 0u;\
    if ( kfsetting->_clean_define != nullptr )\
    {\
        std::tie( operatetype, operatevalue ) = cleanfunction;\
        if ( operatetype != 0u )\
        {\
            kfentity->UpdateData( kfcondition, kfcondition->_data_setting->_value_key_name, operatetype, operatevalue );\
            return KFConditionEnum::UpdateOk;\
        }\
    }\

#define __UPDATE_CONDITION__( updatefunction, limitdata )\
    if ( kfsetting->_condition_define == nullptr )\
    {\
        return KFConditionEnum::UpdateFailed;\
    }\
    if ( !KFUtility::HaveBitMask<uint32>( kfsetting->_limit_mask, KFConditionEnum::LimitStatus ) )\
    {   \
        auto conditionvalue = kfcondition->Get<uint32>( kfcondition->_data_setting->_value_key_name ); \
        if ( KFUtility::CheckOperate( conditionvalue, kfsetting->_done_type, kfsetting->_done_value ) )\
        {\
            return KFConditionEnum::UpdateDone;\
        }\
    }\
    if ( KFUtility::HaveBitMask<uint32>( kfsetting->_limit_mask, KFConditionEnum::LimitPlace ) )\
    {   \
        if ( KFUtility::HaveBitMask<uint32>( limitmask, KFConditionEnum::LimitPlace ) )\
        {   \
            return KFConditionEnum::UpdateFailed; \
        }\
    }\
    std::tie( operatetype, operatevalue ) = updatefunction; \
    if ( operatetype == 0u )\
    {   \
        return KFConditionEnum::UpdateFailed;\
    }\
    if ( !CheckConditionLimit( kfentity, limitdata, kfsetting ) )\
    {   \
        return KFConditionEnum::UpdateFailed; \
    }\
    auto conditionvalue = kfentity->UpdateData( kfcondition, kfcondition->_data_setting->_value_key_name, operatetype, operatevalue ); \
    auto ok = KFUtility::CheckOperate<uint32>( conditionvalue, kfsetting->_done_type, kfsetting->_done_value ); \
    if ( ok )\
    {   \
        return KFConditionEnum::UpdateDone; \
    }\
    return KFConditionEnum::UpdateOk; \

    uint32 KFConditionModule::UpdateAddConditionData( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata )
    {
        // 前置条件
        {
            auto ok = UpdateAddCondition( kfentity, kfcondition, limitmask, kfdata );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcAddConditionValue( kfentity, kfsetting->_clean_define, kfdata ) );
        __UPDATE_CONDITION__( CalcAddConditionValue( kfentity, kfsetting->_condition_define, kfdata ), kfdata );
    }

    uint32 KFConditionModule::UpdateRemoveConditionData( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata )
    {
        // 前置条件
        {
            auto ok = UpdateRemoveCondition( kfentity, kfcondition, limitmask, kfdata );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcRemoveConditionValue( kfentity, kfsetting->_clean_define, kfdata ) );
        __UPDATE_CONDITION__( CalcRemoveConditionValue( kfentity, kfsetting->_condition_define, kfdata ), kfdata );
    }

    uint32 KFConditionModule::UpdateUpdateConditionData( KFEntity* kfentity, KFData* kfcondition, uint32 limitmask, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        // 前置条件
        {
            auto ok = UpdateUpdateCondition( kfentity, kfcondition, limitmask, kfdata, operate, value, nowvalue );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcUpdateConditionValue( kfentity, kfsetting->_clean_define, kfdata, operate, value, nowvalue ) );
        __UPDATE_CONDITION__( CalcUpdateConditionValue( kfentity, kfsetting->_condition_define, kfdata, operate, value, nowvalue ), kfdata->GetParent() );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcAddConditionValue( KFEntity* kfentity, const KFConditionDefine* kfsetting, KFData* kfdata )
    {
        if ( kfdata->_data_setting->_name != kfsetting->_parent_name &&
                kfdata->_data_setting->_logic_name != kfsetting->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_trigger_list )
        {
            if ( trigger._call_type != KFConditionEnum::AddCall )
            {
                continue;
            }

            uint64 value = 0u;
            if ( !kfsetting->_data_name.empty() )
            {
                value = kfdata->Get( kfsetting->_data_name );
            }

            auto ok = trigger.CalcUpdateValue( KFEnum::Set, value, value );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcRemoveConditionValue( KFEntity* kfentity, const KFConditionDefine* kfsetting, KFData* kfdata )
    {
        if ( kfdata->_data_setting->_name != kfsetting->_parent_name &&
                kfdata->_data_setting->_logic_name != kfsetting->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_trigger_list )
        {
            if ( trigger._call_type != KFConditionEnum::RemoveCall )
            {
                continue;
            }

            uint64 value = 0u;
            if ( !kfsetting->_data_name.empty() )
            {
                value = kfdata->Get( kfsetting->_data_name );
            }

            auto ok = trigger.CalcUpdateValue( KFEnum::Dec, value, value );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }


    std::tuple<uint32, uint32> KFConditionModule::CalcUpdateConditionValue( KFEntity* kfentity, const KFConditionDefine* kfsetting, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        if ( ( kfdata->_data_setting->_name != kfsetting->_data_name &&
                kfdata->_data_setting->_logic_name != kfsetting->_data_name )
                ||
                ( kfdata->GetParent()->_data_setting->_name != kfsetting->_parent_name &&
                  kfdata->GetParent()->_data_setting->_logic_name != kfsetting->_parent_name ) )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_trigger_list )
        {
            if ( trigger._call_type != KFConditionEnum::UpdateCall )
            {
                continue;
            }

            auto ok = trigger.CalcUpdateValue( operate, value, nowvalue );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    uint32 KFConditionModule::UpdateConditionData( KFEntity* kfentity, KFData* kfcondition, uint32 conditionid, uint32 operate, uint32 conditionvalue )
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

