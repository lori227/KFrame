#include "KFConditionModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{

#define __CHECK_CONDITION_RESULT__( linktype, result )\
    if ( linktype == KFEnum::And  )\
    {\
        if ( !result )\
        {\
            return false;\
        }\
    }\
    else if ( linktype == KFEnum::Or )\
    {\
        if ( result )\
        {\
            return true;\
        }\
    }\

    bool KFConditionModule::CheckStaticCondition( DataPtr kfobject, const StaticConditionsPtr& kfconditions )
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
        auto result = CheckStaticCondition( kfobject, kfconditions->_condition_list[ conditionindex++ ] );
        for ( auto i = 0u; i < conditioncount && i < linkcount; ++i )
        {
            auto linktype = kfconditions->_link_type[ i ];
            __CHECK_CONDITION_RESULT__( linktype, result );

            auto ok = CheckStaticCondition( kfobject, kfconditions->_condition_list[ conditionindex + i ] );
            __CHECK_CONDITION_RESULT__( linktype, ok );
        }

        return result;
    }

    bool KFConditionModule::CheckStaticCondition( DataPtr kfobject, const KFStaticCondition* kfcondition )
    {
        // 计算左值
        auto leftvalue = CalcExpression( kfobject, kfcondition->_left );

        // 计算右值
        auto rightvalue = CalcExpression( kfobject, kfcondition->_right );

        // 判断左有值
        return KFUtility::CheckOperate( leftvalue, kfcondition->_check_type, rightvalue );
    }

    uint32 KFConditionModule::CalcExpression( DataPtr kfobject, const KFStaticConditionExpression* kfexpression )
    {
        if ( kfexpression->_data_list.empty() )
        {
            return 0u;
        }

        auto datacount = kfexpression->_data_list.size();
        auto operatorcount = kfexpression->_operator_list.size();

        // 取出第一个
        auto dataindex = 0u;
        auto result = CalcConditionData( kfobject, kfexpression->_data_list[ dataindex++ ] );
        for ( auto i = 0u; i < datacount && i < operatorcount; ++i )
        {
            auto operatortype = kfexpression->_operator_list[ i ];
            auto value = CalcConditionData( kfobject, kfexpression->_data_list[ dataindex + i ] );
            result = KFUtility::Operate( result, operatortype, value );
        }

        return result;
    }

    uint32 KFConditionModule::CalcConditionData( DataPtr kfobject, const KFStaticConditionAbstract* kfconditiondata )
    {
        auto result = 0u;
        if ( kfconditiondata->IsConstant() )
        {
            auto kfconstant = ( const KFStaticConditionConstant* )kfconditiondata;
            result = kfconstant->_value;
        }
        else if ( kfconditiondata->IsVariable() )
        {
            auto kfvariable = ( const KFStaticConditionVariable* )kfconditiondata;

            if ( !kfvariable->_parent_name.empty() )
            {
                if ( kfvariable->_data_id == 0u )
                {
                    result = kfobject->Get<uint32>( kfvariable->_parent_name, kfvariable->_data_name );
                }
                else
                {
                    result = kfobject->Get<uint32>( kfvariable->_parent_name, kfvariable->_data_id, kfvariable->_data_name );
                }
            }
            else if ( kfvariable->_data_id == 0u )
            {
                result = kfobject->Get<uint32>( kfvariable->_data_name );
            }
            else
            {
                result = kfobject->GetConfigValue( kfvariable->_data_name, kfvariable->_data_id );
            }
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFConditionModule::CheckStaticConditionLimit( DataPtr kfobject, const KFConditionSetting* kfsetting )
    {
        if ( kfsetting->_static_condition_limit->_conditions_list.empty() )
        {
            return true;
        }

        auto result = ( kfsetting->_static_condition_limit->_check_type == KFEnum::And ) ? true : false;
        for ( auto& limits : kfsetting->_static_condition_limit->_conditions_list )
        {
            auto ok = CheckStaticCondition( kfobject, limits );
            __CHECK_CONDITION_RESULT__( kfsetting->_static_condition_limit->_check_type, ok );
        }
        return result;
    }

    void KFConditionModule::AddCondition( KFEntity* kfentity, DataPtr kfconditionobject, const DynamicConditionGroupPtr& conditiongroup )
    {
        if ( conditiongroup->_id_list.empty() )
        {
            return;
        }

        auto kfconditionrecord = kfconditionobject->Find( __STRING__( condition ) );
        if ( kfconditionrecord == nullptr )
        {
            return;
        }

        kfconditionobject->Set( __STRING__( type ), conditiongroup->_type );
        for ( auto conditionid : conditiongroup->_id_list )
        {
            auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );
            if ( kfsetting == nullptr )
            {
                __LOG_ERROR__( "condition=[{}] can't find setting", conditionid );
                _kf_display->SendToClient( kfentity, KFMsg::ConditionSettingError, conditionid );
                continue;
            }

            auto kfcondition = kfentity->CreateData( kfconditionrecord );

            // 前置条件
            AddCondition( kfentity, kfcondition, kfsetting->_dynamic_condition_limit );

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

    bool KFConditionModule::InitCondition( KFEntity* kfentity, DataPtr kfconditionobject, bool update )
    {
        __CHECK_CONDITION__( InitConditionData( kfentity, kfcondition, update ) );
    }

    uint32 KFConditionModule::InitConditionData( KFEntity* kfentity, DataPtr kfcondition, bool update )
    {
        // 限制条件初始化
        {
            auto ok = InitCondition( kfentity, kfcondition, update );
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
            _kf_display->SendToClient( kfentity, KFMsg::ConditionSettingError, conditionid );
            return KFConditionEnum::UpdateDone;
        }

        if ( kfsetting->_condition._condition_define == nullptr ||
                kfsetting->_condition._condition_define->_init_calc_type == KFConditionEnum::InitNull )
        {
            return KFConditionEnum::UpdateFailed;
        }

        auto conditionvalue = 0u;
        switch ( kfsetting->_condition._condition_define->_init_calc_type )
        {
        case KFConditionEnum::InitCalcCount:
            conditionvalue = InitCalcRecordCount( kfentity, kfcondition, kfsetting );
            break;
        case KFConditionEnum::InitGetValue:
            conditionvalue = InitCalcGetValue( kfentity, kfcondition, kfsetting );
            break;
        case KFConditionEnum::InitCheckExist:
            conditionvalue = InitCalcRecordExist( kfentity, kfcondition, kfsetting );
            break;
        case KFConditionEnum::InitCalcValue:
            conditionvalue = InitCalcRecordValue( kfentity, kfcondition, kfsetting );
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
            kfentity->UpdateObject( kfcondition, kfcondition->_data_setting->_value_key_name, KFEnum::Set, conditionvalue );
        }

        auto ok = KFUtility::CheckOperate( conditionvalue, kfsetting->_done_type, kfsetting->_done_value );
        if ( ok )
        {
            return KFConditionEnum::UpdateDone;
        }

        return KFConditionEnum::UpdateOk;
    }

    std::list< DataPtr >& KFConditionModule::FindRecordList( KFEntity* kfentity, const std::string& name )
    {
        static std::list< DataPtr > _recrods;
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

    uint32 KFConditionModule::InitCalcRecordCount( KFEntity* kfentity, DataPtr kfcondition, const KFConditionSetting* kfsetting )
    {
        auto recordcount = 0u;
        auto& kfrecords = FindRecordList( kfentity, kfsetting->_condition._condition_define->_parent_name );
        for ( auto kfrecord : kfrecords )
        {
            for ( auto kfchild = kfrecord->First(); kfchild != nullptr; kfchild = kfrecord->Next() )
            {
                auto ok = CheckStaticConditionLimit( kfchild, kfsetting );
                if ( ok )
                {
                    ++recordcount;
                    SaveConditionDataUUid( kfentity, kfcondition, kfchild, kfsetting->_condition._condition_define );
                }
            }
        }

        return recordcount;
    }


    uint32 KFConditionModule::InitCalcRecordValue( KFEntity* kfentity, DataPtr kfcondition, const KFConditionSetting* kfsetting )
    {
        auto recordvalue = 0u;
        auto& kfrecords = FindRecordList( kfentity, kfsetting->_condition._condition_define->_parent_name );
        for ( auto kfrecord : kfrecords )
        {
            for ( auto kfchild = kfrecord->First(); kfchild != nullptr; kfchild = kfrecord->Next() )
            {
                auto ok = CheckStaticConditionLimit( kfchild, kfsetting );
                if ( ok )
                {
                    SaveConditionDataUUid( kfentity, kfcondition, kfcondition, kfsetting->_condition._condition_define );
                    recordvalue += kfchild->Get<uint32>( kfchild->_data_setting->_value_key_name );
                }
            }
        }

        return recordvalue;
    }

    uint32 KFConditionModule::InitCalcRecordExist( KFEntity* kfentity, DataPtr kfcondition, const KFConditionSetting* kfsetting )
    {
        auto& kfrecords = FindRecordList( kfentity, kfsetting->_condition._condition_define->_parent_name );
        //for ( auto kfrecord : kfrecords )
        //{
        //    for ( auto& limits : kfsetting->_static_condition_limit->_conditions_list )
        //    {
        //        for ( auto& limit : limits._limit )
        //        {
        //            if ( limit._data_name == __STRING__( id ) )
        //            {
        //                auto kfdata = kfrecord->Find( limit._data_value );
        //                if ( kfdata != nullptr )
        //                {
        //                    return 1u;
        //                }
        //            }
        //        }
        //    }
        //}

        return 0u;
    }

    uint32 KFConditionModule::InitCalcGetValue( KFEntity* kfentity, DataPtr kfcondition, const KFConditionSetting* kfsetting )
    {
        if ( kfsetting->_condition._condition_define->_parent_name == __STRING__( player ) )
        {
            return kfentity->Get<uint32>( kfsetting->_condition._condition_define->_data_name );
        }

        // 集合
        auto recordvalue = 0u;
        auto& kfrecords = FindRecordList( kfentity, kfsetting->_condition._condition_define->_parent_name );
        for ( auto kfrecord : kfrecords )
        {
            for ( auto kfchild = kfrecord->First(); kfchild != nullptr; kfchild = kfrecord->Next() )
            {
                auto ok = CheckStaticConditionLimit( kfchild, kfsetting );
                if ( ok )
                {
                    recordvalue = kfchild->Get<uint32>( kfsetting->_condition._condition_define->_data_name );
                    break;
                }
            }
        }

        return recordvalue;
    }

    uint32 KFConditionModule::CheckConditionData( KFEntity* kfentity, DataPtr kfcondition )
    {
        auto conditionid = kfcondition->Get( kfcondition->_data_setting->_config_key_name );
        auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );
        if ( kfsetting == nullptr || kfsetting->_condition._condition_define == nullptr )
        {
            _kf_display->SendToClient( kfentity, KFMsg::ConditionSettingError, conditionid );
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

    bool KFConditionModule::CheckCondition( KFEntity* kfentity, DataPtr kfconditionobject )
    {
        __CHECK_CONDITION__( CheckConditionData( kfentity, kfcondition ) );
    }

    bool KFConditionModule::UpdateCondition( KFEntity* kfentity, DataPtr kfconditionobject, uint32 conditionid, uint32 operate, uint32 conditionvalue )
    {
        __CHECK_CONDITION__( UpdateConditionData( kfentity, kfcondition, conditionid, operate, conditionvalue ) );
    }

    bool KFConditionModule::UpdateAddCondition( KFEntity* kfentity, DataPtr kfconditionobject, DataPtr kfdata )
    {
        __CHECK_CONDITION__( UpdateAddConditionData( kfentity, kfcondition, kfdata ) );
    }

    bool KFConditionModule::UpdateRemoveCondition( KFEntity* kfentity, DataPtr kfconditionobject, DataPtr kfdata )
    {
        __CHECK_CONDITION__( UpdateRemoveConditionData( kfentity, kfcondition, kfdata ) );
    }

    bool KFConditionModule::UpdateUpdateCondition( KFEntity* kfentity, DataPtr kfconditionobject, DataPtr kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        __CHECK_CONDITION__( UpdateUpdateConditionData( kfentity, kfcondition, kfdata, operate, value, nowvalue ) );
    }

#define __CLEAN_CONDITION__( cleanfunction )\
    auto conditionid = kfcondition->Get( kfcondition->_data_setting->_config_key_name );\
    auto kfsetting = KFConditionConfig::Instance()->FindSetting( conditionid );\
    if ( kfsetting == nullptr )\
    {\
        __LOG_ERROR__( "condition=[{}] can't find setting", conditionid );\
        _kf_display->SendToClient( kfentity, KFMsg::ConditionSettingError, conditionid );\
        return KFConditionEnum::UpdateDone;\
    }\
    auto operatetype = 0u;\
    auto operate_value = 0u;\
    if ( kfsetting->_clean._condition_define != nullptr )\
    {\
        std::tie( operatetype, operate_value ) = cleanfunction;\
        if ( operatetype != 0u )\
        {\
            kfentity->UpdateObject( kfcondition, kfcondition->_data_setting->_value_key_name, operatetype, operate_value );\
            return KFConditionEnum::UpdateOk;\
        }\
    }\

#define __UPDATE_CONDITION__( updatefunction, limitdata )\
    if ( kfsetting->_condition._condition_define == nullptr )\
    {\
        return KFConditionEnum::UpdateFailed;\
    }\
    auto conditionvalue = kfcondition->Get<uint32>( kfcondition->_data_setting->_value_key_name ); \
    if ( KFUtility::CheckOperate( conditionvalue, kfsetting->_done_type, kfsetting->_done_value ) )\
    {\
        return KFConditionEnum::UpdateDone;\
    }\
    if ( CheckUUidInConditionData( kfentity, kfcondition, limitdata, kfsetting->_condition._condition_define ) )\
    {\
        return KFConditionEnum::UpdateOk; \
    }\
    std::tie( operatetype, operate_value ) = updatefunction; \
    if ( operatetype == 0u )\
    {   \
        return KFConditionEnum::UpdateFailed;\
    }\
    if ( !CheckStaticConditionLimit( limitdata, kfsetting ) )\
    {   \
        return KFConditionEnum::UpdateFailed; \
    }\
    SaveConditionDataUUid( kfentity, kfcondition, limitdata, kfsetting->_condition._condition_define );\
    auto finalvalue = kfentity->UpdateObject( kfcondition, kfcondition->_data_setting->_value_key_name, operatetype, operate_value ); \
    auto ok = KFUtility::CheckOperate<uint32>( finalvalue, kfsetting->_done_type, kfsetting->_done_value ); \
    if ( ok )\
    {   \
        return KFConditionEnum::UpdateDone; \
    }\
    return KFConditionEnum::UpdateOk; \

    uint32 KFConditionModule::UpdateAddConditionData( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata )
    {
        // 前置条件
        {
            auto ok = UpdateAddCondition( kfentity, kfcondition, kfdata );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcAddConditionValue( kfentity, kfsetting->_clean._condition_define, kfdata ) );
        __UPDATE_CONDITION__( CalcAddConditionValue( kfentity, kfsetting->_condition._condition_define, kfdata ), kfdata );
    }

    uint32 KFConditionModule::UpdateRemoveConditionData( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata )
    {
        // 前置条件
        {
            auto ok = UpdateRemoveCondition( kfentity, kfcondition, kfdata );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcRemoveConditionValue( kfentity, kfsetting->_clean._condition_define, kfdata ) );
        __UPDATE_CONDITION__( CalcRemoveConditionValue( kfentity, kfsetting->_condition._condition_define, kfdata ), kfdata );
    }

    uint32 KFConditionModule::UpdateUpdateConditionData( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        // 前置条件
        {
            auto ok = UpdateUpdateCondition( kfentity, kfcondition, kfdata, operate, value, nowvalue );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcUpdateConditionValue( kfentity, kfsetting->_clean._condition_define, kfdata, operate, value, nowvalue ) );
        __UPDATE_CONDITION__( CalcUpdateConditionValue( kfentity, kfsetting->_condition._condition_define, kfdata, operate, value, nowvalue ), kfdata->GetParent() );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcAddConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, DataPtr kfdata )
    {
        if ( kfdata->_data_setting->_name != kfsetting->_parent_name &&
                kfdata->_data_setting->_logic_name != kfsetting->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_condition_trigger )
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

            auto ok = CalcTriggerUpdateValue( &trigger, KFEnum::Set, value, value );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcRemoveConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, DataPtr kfdata )
    {
        if ( kfdata->_data_setting->_name != kfsetting->_parent_name &&
                kfdata->_data_setting->_logic_name != kfsetting->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_condition_trigger )
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

            auto ok = CalcTriggerUpdateValue( &trigger, KFEnum::Dec, value, value );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcUpdateConditionValue( KFEntity* kfentity, const KFConditionDefineSetting* kfsetting, DataPtr kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        if ( ( kfdata->GetParent()->_data_setting->_name != kfsetting->_parent_name && kfdata->GetParent()->_data_setting->_logic_name != kfsetting->_parent_name ) )
        {
            return std::make_tuple( 0u, 0u );
        }

        if ( !kfsetting->_data_name.empty() &&
                ( kfdata->_data_setting->_name != kfsetting->_data_name && kfdata->_data_setting->_logic_name != kfsetting->_data_name ) )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : kfsetting->_condition_trigger )
        {
            if ( trigger._call_type != KFConditionEnum::UpdateCall )
            {
                continue;
            }

            auto ok = CalcTriggerUpdateValue( &trigger, operate, value, nowvalue );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    uint32 KFConditionModule::UpdateConditionData( KFEntity* kfentity, DataPtr kfcondition, uint32 conditionid, uint32 operate, uint32 conditionvalue )
    {
        auto updateconditionid = kfcondition->GetKeyID();
        auto kfsetting = KFConditionConfig::Instance()->FindSetting( updateconditionid );
        if ( kfsetting == nullptr || kfsetting->_condition._condition_define == nullptr )
        {
            _kf_display->SendToClient( kfentity, KFMsg::ConditionSettingError, updateconditionid );
            return KFConditionEnum::UpdateFailed;
        }

        if ( updateconditionid == conditionid )
        {
            conditionvalue = kfentity->UpdateObject( kfcondition, kfcondition->_data_setting->_value_key_name, operate, conditionvalue );
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

    void KFConditionModule::SaveConditionDataUUid( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata, const KFConditionDefineSetting* kfsetting )
    {
        if ( !kfsetting->_save_uuid )
        {
            return;
        }

        auto kfuuidrecord = kfcondition->Find( __STRING__( uuid ) );
        if ( kfuuidrecord == nullptr )
        {
            return;
        }

        auto uuid = kfdata->GetKeyID();
        auto kfuuid = kfentity->CreateData( kfuuidrecord );
        kfuuid->Set( __STRING__( id ), uuid );
        kfuuid->Set( __STRING__( value ), uuid );
        kfuuidrecord->Add( uuid, kfuuid );
    }

    bool KFConditionModule::CheckUUidInConditionData( KFEntity* kfentity, DataPtr kfcondition, DataPtr kfdata, const KFConditionDefineSetting* kfsetting )
    {
        if ( !kfsetting->_save_uuid )
        {
            return false;
        }

        auto uuid = kfdata->GetKeyID();
        auto kfuuid = kfcondition->Find( __STRING__( uuid ), uuid );
        return kfuuid != nullptr;
    }

    bool KFConditionModule::CalcTriggerUpdateValue( const ConditionTrigger* trigger, uint64 operate, uint64& operate_value, uint64 nowvalue )
    {
        if ( trigger->_trigger_type != 0u && trigger->_trigger_type != operate )
        {
            return false;
        }

        // 触发值
        if ( trigger->_trigger_check != 0u )
        {
            auto triggervalue = operate_value;
            if ( trigger->_trigger_use == KFConditionEnum::UseFinal )
            {
                triggervalue = nowvalue;
            }

            auto ok = KFUtility::CheckOperate<uint64>( triggervalue, trigger->_trigger_check, trigger->_trigger_value );
            if ( !ok )
            {
                return false;
            }
        }

        // 最终使用值
        switch ( trigger->_use_type )
        {
        case KFConditionEnum::UseSetting:
            operate_value = trigger->_use_value;
            break;
        case KFConditionEnum::UseFinal:
            operate_value = nowvalue;
            break;
        default:
            break;
        }

        return true;
    }
}

