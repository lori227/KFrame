#include "KFConditionModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{

#define __CHECK_CONDITION_RESULT__( link_type, result )\
    if ( link_type == KFEnum::And  )\
    {\
        if ( !result )\
        {\
            return false;\
        }\
    }\
    else if ( link_type == KFEnum::Or )\
    {\
        if ( result )\
        {\
            return true;\
        }\
    }\

    bool KFConditionModule::CheckStaticCondition( DataPtr object_data, const StaticConditionsPtr& static_conditions )
    {
        if ( !static_conditions->IsValid() )
        {
            return false;
        }

        if ( static_conditions->IsEmpty() )
        {
            return true;
        }

        auto link_count = static_conditions->_link_type.size();
        auto condition_count = static_conditions->_condition_list.size();

        // 取出第一个
        auto condition_index = 0u;
        auto result = CheckStaticCondition( object_data, static_conditions->_condition_list[ condition_index++ ] );
        for ( auto i = 0u; i < condition_count && i < link_count; ++i )
        {
            auto link_type = static_conditions->_link_type[ i ];
            __CHECK_CONDITION_RESULT__( link_type, result );

            auto ok = CheckStaticCondition( object_data, static_conditions->_condition_list[ condition_index + i ] );
            __CHECK_CONDITION_RESULT__( link_type, ok );
        }

        return result;
    }

    bool KFConditionModule::CheckStaticCondition( DataPtr object_data, std::shared_ptr<const KFStaticCondition> static_condition )
    {
        // 计算左值
        auto left_value = CalcExpression( object_data, static_condition->_left );

        // 计算右值
        auto right_value = CalcExpression( object_data, static_condition->_right );

        // 判断左有值
        return KFUtility::CheckOperate( left_value, static_condition->_check_type, right_value );
    }

    uint32 KFConditionModule::CalcExpression( DataPtr object_data, std::shared_ptr<const KFStaticConditionExpression> static_expression )
    {
        if ( static_expression->_data_list.empty() )
        {
            return 0u;
        }

        auto data_count = static_expression->_data_list.size();
        auto operator_count = static_expression->_operator_list.size();

        // 取出第一个
        auto data_index = 0u;
        auto result = CalcConditionData( object_data, static_expression->_data_list[ data_index++ ] );
        for ( auto i = 0u; i < data_count && i < operator_count; ++i )
        {
            auto operator_type = static_expression->_operator_list[ i ];
            auto value = CalcConditionData( object_data, static_expression->_data_list[ data_index + i ] );
            result = KFUtility::Operate( result, operator_type, value );
        }

        return result;
    }

    uint32 KFConditionModule::CalcConditionData( DataPtr object_data, std::shared_ptr<const KFStaticConditionAbstract> condition_data )
    {
        auto result = 0u;
        if ( condition_data->IsConstant() )
        {
            auto condition_constant = std::static_pointer_cast<const KFStaticConditionConstant>( condition_data );
            result = condition_constant->_value;
        }
        else if ( condition_data->IsVariable() )
        {
            auto condition_variable = std::static_pointer_cast<const KFStaticConditionVariable>( condition_data );

            if ( !condition_variable->_parent_name.empty() )
            {
                if ( condition_variable->_data_id == 0u )
                {
                    result = object_data->Get<uint32>( condition_variable->_parent_name, condition_variable->_data_name );
                }
                else
                {
                    result = object_data->Get<uint32>( condition_variable->_parent_name, condition_variable->_data_id, condition_variable->_data_name );
                }
            }
            else if ( condition_variable->_data_id == 0u )
            {
                result = object_data->Get<uint32>( condition_variable->_data_name );
            }
            else
            {
                result = object_data->GetConfigValue( condition_variable->_data_name, condition_variable->_data_id );
            }
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFConditionModule::CheckStaticConditionLimit( DataPtr object_data, std::shared_ptr<const KFConditionSetting> setting )
    {
        if ( setting->_static_condition_limit->_conditions_list.empty() )
        {
            return true;
        }

        auto result = ( setting->_static_condition_limit->_check_type == KFEnum::And ) ? true : false;
        for ( auto& limits : setting->_static_condition_limit->_conditions_list )
        {
            auto ok = CheckStaticCondition( object_data, limits );
            __CHECK_CONDITION_RESULT__( setting->_static_condition_limit->_check_type, ok );
        }
        return result;
    }

    void KFConditionModule::AddCondition( EntityPtr entity, DataPtr condition_object, const DynamicConditionGroupPtr& condition_group )
    {
        if ( condition_group->_id_list.empty() )
        {
            return;
        }

        auto condition_record = condition_object->Find( __STRING__( condition ) );
        if ( condition_record == nullptr )
        {
            return;
        }

        condition_object->Operate( __STRING__( type ), KFEnum::Set, condition_group->_type );
        for ( auto condition_id : condition_group->_id_list )
        {
            auto setting = KFConditionConfig::Instance()->FindSetting( condition_id );
            if ( setting == nullptr )
            {
                __LOG_ERROR__( "condition=[{}] can't find setting", condition_id );
                _kf_display->SendToClient( entity, KFMsg::ConditionSettingError, condition_id );
                continue;
            }

            auto condition_data = entity->CreateData( condition_record );

            // 前置条件
            AddCondition( entity, condition_data, setting->_dynamic_condition_limit );

            condition_data->Operate( __STRING__( id ), KFEnum::Set, condition_id );
            condition_record->Add( condition_id, condition_data );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __CHECK_CONDITION__( check_function )\
    if ( condition_object == nullptr )\
    {\
        return false;\
    }\
    auto condition_type = condition_object->Get<uint32>( __STRING__( type ) );\
    if ( condition_type == 0u )\
    {\
        return true;\
    }\
    auto condition_record = condition_object->Find( __STRING__( condition ) );\
    if ( condition_record == nullptr || condition_record->Size() == 0u )\
    {\
        return true;\
    }\
    bool complete = false;\
    switch ( condition_type )\
    {\
    case KFEnum::Or:	\
    {\
        complete = false;\
        for ( auto condition_data = condition_record->First(); condition_data != nullptr; condition_data = condition_record->Next() )\
        {\
            auto result = check_function;\
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
        for ( auto condition_data = condition_record->First(); condition_data != nullptr; condition_data = condition_record->Next() )\
        {\
            auto result = check_function;\
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

    bool KFConditionModule::InitCondition( EntityPtr entity, DataPtr condition_object, bool update )
    {
        __CHECK_CONDITION__( InitConditionData( entity, condition_data, update ) );
    }

    uint32 KFConditionModule::InitConditionData( EntityPtr entity, DataPtr condition_data, bool update )
    {
        // 限制条件初始化
        {
            auto ok = InitCondition( entity, condition_data, update );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        auto condition_id = condition_data->Get( condition_data->_data_setting->_config_key_name );
        auto setting = KFConditionConfig::Instance()->FindSetting( condition_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "condition=[{}] can't find setting", condition_id );
            _kf_display->SendToClient( entity, KFMsg::ConditionSettingError, condition_id );
            return KFConditionEnum::UpdateDone;
        }

        if ( setting->_condition._condition_define == nullptr ||
                setting->_condition._condition_define->_init_calc_type == KFConditionEnum::InitNull )
        {
            return KFConditionEnum::UpdateFailed;
        }

        auto condition_value = 0u;
        switch ( setting->_condition._condition_define->_init_calc_type )
        {
        case KFConditionEnum::InitCalcCount:
            condition_value = InitCalcRecordCount( entity, condition_data, setting );
            break;
        case KFConditionEnum::InitGetValue:
            condition_value = InitCalcGetValue( entity, condition_data, setting );
            break;
        case KFConditionEnum::InitCheckExist:
            condition_value = InitCalcRecordExist( entity, condition_data, setting );
            break;
        case KFConditionEnum::InitCalcValue:
            condition_value = InitCalcRecordValue( entity, condition_data, setting );
            break;
        default:
            break;
        }

        if ( !update )
        {
            condition_data->Operate( condition_data->_data_setting->_value_key_name, KFEnum::Set, condition_value );
        }
        else
        {
            entity->UpdateObject( condition_data, condition_data->_data_setting->_value_key_name, KFEnum::Set, condition_value );
        }

        auto ok = KFUtility::CheckOperate( condition_value, setting->_done_type, setting->_done_value );
        if ( ok )
        {
            return KFConditionEnum::UpdateDone;
        }

        return KFConditionEnum::UpdateOk;
    }

    std::list<DataPtr>& KFConditionModule::FindRecordList( EntityPtr entity, const std::string& name )
    {
        static std::list<DataPtr> _record_data_list;
        _record_data_list.clear();

        auto record_data = entity->Find( name );
        if ( record_data != nullptr )
        {
            _record_data_list.push_back( record_data );
        }
        else
        {
            for ( auto child_data = entity->First(); child_data != nullptr; child_data = entity->Next() )
            {
                if ( child_data->_data_setting->_logic_name == name )
                {
                    _record_data_list.push_back( child_data );
                }
            }
        }

        return _record_data_list;
    }

    uint32 KFConditionModule::InitCalcRecordCount( EntityPtr entity, DataPtr condition_data, std::shared_ptr<const KFConditionSetting> setting )
    {
        auto record_count = 0u;
        auto& record_data_list = FindRecordList( entity, setting->_condition._condition_define->_parent_name );
        for ( auto record_data : record_data_list )
        {
            for ( auto child_data = record_data->First(); child_data != nullptr; child_data = record_data->Next() )
            {
                auto ok = CheckStaticConditionLimit( child_data, setting );
                if ( ok )
                {
                    ++record_count;
                    SaveConditionDataUUid( entity, condition_data, child_data, setting->_condition._condition_define );
                }
            }
        }

        return record_count;
    }


    uint32 KFConditionModule::InitCalcRecordValue( EntityPtr entity, DataPtr condition_data, std::shared_ptr<const KFConditionSetting> setting )
    {
        auto record_value = 0u;
        auto& record_data_list = FindRecordList( entity, setting->_condition._condition_define->_parent_name );
        for ( auto record_data : record_data_list )
        {
            for ( auto child_data = record_data->First(); child_data != nullptr; child_data = record_data->Next() )
            {
                auto ok = CheckStaticConditionLimit( child_data, setting );
                if ( ok )
                {
                    SaveConditionDataUUid( entity, condition_data, condition_data, setting->_condition._condition_define );
                    record_value += child_data->Get<uint32>( child_data->_data_setting->_value_key_name );
                }
            }
        }

        return record_value;
    }

    uint32 KFConditionModule::InitCalcRecordExist( EntityPtr entity, DataPtr condition_data, std::shared_ptr<const KFConditionSetting> setting )
    {
        auto& record_data_list = FindRecordList( entity, setting->_condition._condition_define->_parent_name );
        //for ( auto record_data : record_data_list )
        //{
        //    for ( auto& limits : setting->_static_condition_limit->_conditions_list )
        //    {
        //        for ( auto& limit : limits._limit )
        //        {
        //            if ( limit._data_name == __STRING__( id ) )
        //            {
        //                auto data = record_data->Find( limit._data_value );
        //                if ( data != nullptr )
        //                {
        //                    return 1u;
        //                }
        //            }
        //        }
        //    }
        //}

        return 0u;
    }

    uint32 KFConditionModule::InitCalcGetValue( EntityPtr entity, DataPtr condition_data, std::shared_ptr<const KFConditionSetting> setting )
    {
        if ( setting->_condition._condition_define->_parent_name == __STRING__( player ) )
        {
            return entity->Get<uint32>( setting->_condition._condition_define->_data_name );
        }

        // 集合
        auto& record_data_list = FindRecordList( entity, setting->_condition._condition_define->_parent_name );
        for ( auto record_data : record_data_list )
        {
            for ( auto child_data = record_data->First(); child_data != nullptr; child_data = record_data->Next() )
            {
                auto ok = CheckStaticConditionLimit( child_data, setting );
                if ( ok )
                {
                    return child_data->Get<uint32>( setting->_condition._condition_define->_data_name );
                }
            }
        }

        return 0u;
    }

    uint32 KFConditionModule::CheckConditionData( EntityPtr entity, DataPtr condition_data )
    {
        auto condition_id = condition_data->Get( condition_data->_data_setting->_config_key_name );
        auto setting = KFConditionConfig::Instance()->FindSetting( condition_id );
        if ( setting == nullptr || setting->_condition._condition_define == nullptr )
        {
            _kf_display->SendToClient( entity, KFMsg::ConditionSettingError, condition_id );
            return KFConditionEnum::UpdateFailed;
        }

        auto condition_value = condition_data->Get<uint32>( condition_data->_data_setting->_value_key_name );
        auto ok = KFUtility::CheckOperate<uint32>( condition_value, setting->_done_type, setting->_done_value );
        if ( ok )
        {
            return KFConditionEnum::UpdateDone;
        }

        return KFConditionEnum::UpdateOk;
    }

    bool KFConditionModule::CheckCondition( EntityPtr entity, DataPtr condition_object )
    {
        __CHECK_CONDITION__( CheckConditionData( entity, condition_data ) );
    }

    bool KFConditionModule::UpdateCondition( EntityPtr entity, DataPtr condition_object, uint32 condition_id, uint32 operate, uint32 condition_value )
    {
        __CHECK_CONDITION__( UpdateConditionData( entity, condition_data, condition_id, operate, condition_value ) );
    }

    bool KFConditionModule::UpdateAddCondition( EntityPtr entity, DataPtr condition_object, DataPtr data )
    {
        __CHECK_CONDITION__( UpdateAddConditionData( entity, condition_data, data ) );
    }

    bool KFConditionModule::UpdateRemoveCondition( EntityPtr entity, DataPtr condition_object, DataPtr data )
    {
        __CHECK_CONDITION__( UpdateRemoveConditionData( entity, condition_data, data ) );
    }

    bool KFConditionModule::UpdateUpdateCondition( EntityPtr entity, DataPtr condition_object, DataPtr data, uint32 operate, uint64 value, uint64 now_value )
    {
        __CHECK_CONDITION__( UpdateUpdateConditionData( entity, condition_data, data, operate, value, now_value ) );
    }

#define __CLEAN_CONDITION__( clean_function )\
    auto condition_id = condition_data->Get( condition_data->_data_setting->_config_key_name );\
    auto setting = KFConditionConfig::Instance()->FindSetting( condition_id );\
    if ( setting == nullptr )\
    {\
        __LOG_ERROR__( "condition=[{}] can't find setting", condition_id );\
        _kf_display->SendToClient( entity, KFMsg::ConditionSettingError, condition_id );\
        return KFConditionEnum::UpdateDone;\
    }\
    auto operate_type = 0u;\
    auto operate_value = 0u;\
    if ( setting->_clean._condition_define != nullptr )\
    {\
        std::tie( operate_type, operate_value ) = clean_function;\
        if ( operate_type != 0u )\
        {\
            entity->UpdateObject( condition_data, condition_data->_data_setting->_value_key_name, operate_type, operate_value );\
            return KFConditionEnum::UpdateOk;\
        }\
    }\

#define __UPDATE_CONDITION__( update_function, limit_data )\
    if ( setting->_condition._condition_define == nullptr )\
    {\
        return KFConditionEnum::UpdateFailed;\
    }\
    auto condition_value = condition_data->Get<uint32>( condition_data->_data_setting->_value_key_name ); \
    if ( KFUtility::CheckOperate( condition_value, setting->_done_type, setting->_done_value ) )\
    {\
        return KFConditionEnum::UpdateDone;\
    }\
    if ( CheckUUidInConditionData( entity, condition_data, limit_data, setting->_condition._condition_define ) )\
    {\
        return KFConditionEnum::UpdateOk; \
    }\
    std::tie( operate_type, operate_value ) = update_function; \
    if ( operate_type == 0u )\
    {   \
        return KFConditionEnum::UpdateFailed;\
    }\
    if ( !CheckStaticConditionLimit( limit_data, setting ) )\
    {   \
        return KFConditionEnum::UpdateFailed; \
    }\
    SaveConditionDataUUid( entity, condition_data, limit_data, setting->_condition._condition_define );\
    auto final_value = entity->UpdateObject( condition_data, condition_data->_data_setting->_value_key_name, operate_type, operate_value ); \
    auto ok = KFUtility::CheckOperate<uint32>( final_value, setting->_done_type, setting->_done_value ); \
    if ( ok )\
    {   \
        return KFConditionEnum::UpdateDone; \
    }\
    return KFConditionEnum::UpdateOk; \

    uint32 KFConditionModule::UpdateAddConditionData( EntityPtr entity, DataPtr condition_data, DataPtr data )
    {
        // 前置条件
        {
            auto ok = UpdateAddCondition( entity, condition_data, data );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcAddConditionValue( entity, setting->_clean._condition_define, data ) );
        __UPDATE_CONDITION__( CalcAddConditionValue( entity, setting->_condition._condition_define, data ), data );
    }

    uint32 KFConditionModule::UpdateRemoveConditionData( EntityPtr entity, DataPtr condition_data, DataPtr data )
    {
        // 前置条件
        {
            auto ok = UpdateRemoveCondition( entity, condition_data, data );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcRemoveConditionValue( entity, setting->_clean._condition_define, data ) );
        __UPDATE_CONDITION__( CalcRemoveConditionValue( entity, setting->_condition._condition_define, data ), data );
    }

    uint32 KFConditionModule::UpdateUpdateConditionData( EntityPtr entity, DataPtr condition_data, DataPtr data, uint32 operate, uint64 value, uint64 now_value )
    {
        // 前置条件
        {
            auto ok = UpdateUpdateCondition( entity, condition_data, data, operate, value, now_value );
            if ( !ok )
            {
                return KFConditionEnum::UpdateLimit;
            }
        }

        __CLEAN_CONDITION__( CalcUpdateConditionValue( entity, setting->_clean._condition_define, data, operate, value, now_value ) );
        __UPDATE_CONDITION__( CalcUpdateConditionValue( entity, setting->_condition._condition_define, data, operate, value, now_value ), data->GetParent()->GetDataPtr() );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcAddConditionValue( EntityPtr entity, std::shared_ptr<const KFConditionDefineSetting> setting, DataPtr data )
    {
        if ( data->_data_setting->_name != setting->_parent_name &&
                data->_data_setting->_logic_name != setting->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : setting->_condition_trigger )
        {
            if ( trigger._call_type != KFConditionEnum::AddCall )
            {
                continue;
            }

            uint64 value = 0u;
            if ( !setting->_data_name.empty() )
            {
                value = data->Get( setting->_data_name );
            }

            auto ok = CalcTriggerUpdateValue( &trigger, KFEnum::Set, value, value );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcRemoveConditionValue( EntityPtr entity, std::shared_ptr<const KFConditionDefineSetting> setting, DataPtr data )
    {
        if ( data->_data_setting->_name != setting->_parent_name &&
                data->_data_setting->_logic_name != setting->_parent_name )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : setting->_condition_trigger )
        {
            if ( trigger._call_type != KFConditionEnum::RemoveCall )
            {
                continue;
            }

            uint64 value = 0u;
            if ( !setting->_data_name.empty() )
            {
                value = data->Get( setting->_data_name );
            }

            auto ok = CalcTriggerUpdateValue( &trigger, KFEnum::Dec, value, value );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    std::tuple<uint32, uint32> KFConditionModule::CalcUpdateConditionValue( EntityPtr entity, std::shared_ptr<const KFConditionDefineSetting> setting, DataPtr data, uint32 operate, uint64 value, uint64 now_value )
    {
        if ( ( data->GetParent()->_data_setting->_name != setting->_parent_name && 
            data->GetParent()->_data_setting->_logic_name != setting->_parent_name ) )
        {
            return std::make_tuple( 0u, 0u );
        }

        if ( !setting->_data_name.empty() &&
                ( data->_data_setting->_name != setting->_data_name && data->_data_setting->_logic_name != setting->_data_name ) )
        {
            return std::make_tuple( 0u, 0u );
        }

        for ( auto& trigger : setting->_condition_trigger )
        {
            if ( trigger._call_type != KFConditionEnum::UpdateCall )
            {
                continue;
            }

            auto ok = CalcTriggerUpdateValue( &trigger, operate, value, now_value );
            if ( ok )
            {
                return std::make_tuple( trigger._use_operate, value );
            }
        }

        return std::make_tuple( 0u, 0u );
    }

    uint32 KFConditionModule::UpdateConditionData( EntityPtr entity, DataPtr condition_data, uint32 condition_id, uint32 operate, uint32 condition_value )
    {
        auto update_condition_id = condition_data->GetKeyID();
        auto setting = KFConditionConfig::Instance()->FindSetting( update_condition_id );
        if ( setting == nullptr || setting->_condition._condition_define == nullptr )
        {
            _kf_display->SendToClient( entity, KFMsg::ConditionSettingError, update_condition_id );
            return KFConditionEnum::UpdateFailed;
        }

        if ( update_condition_id == condition_id )
        {
            condition_value = entity->UpdateObject( condition_data, condition_data->_data_setting->_value_key_name, operate, condition_value );
        }
        else
        {
            condition_value = condition_data->Get<uint32>( condition_data->_data_setting->_value_key_name );
        }

        auto ok = KFUtility::CheckOperate<uint32>( condition_value, setting->_done_type, setting->_done_value );
        if ( ok )
        {
            return KFConditionEnum::UpdateDone;
        }

        return KFConditionEnum::UpdateOk;
    }

    void KFConditionModule::SaveConditionDataUUid( EntityPtr entity, DataPtr condition_data, DataPtr data, std::shared_ptr<const KFConditionDefineSetting> setting )
    {
        if ( !setting->_save_uuid )
        {
            return;
        }

        auto uuid_record = condition_data->Find( __STRING__( uuid ) );
        if ( uuid_record == nullptr )
        {
            return;
        }

        auto uuid_data = entity->CreateData( uuid_record );

        auto uuid = data->GetKeyID();
        uuid_data->Operate( __STRING__( id ), KFEnum::Set, uuid );
        uuid_data->Operate( __STRING__( value ), KFEnum::Set, uuid );
        uuid_record->Add( uuid, uuid_data );
    }

    bool KFConditionModule::CheckUUidInConditionData( EntityPtr entity, DataPtr condition_data, DataPtr data, std::shared_ptr<const KFConditionDefineSetting> setting )
    {
        if ( !setting->_save_uuid )
        {
            return false;
        }

        auto uuid_data = condition_data->Find( __STRING__( uuid ), data->GetKeyID() );
        return uuid_data != nullptr;
    }

    bool KFConditionModule::CalcTriggerUpdateValue( const ConditionTrigger* trigger, uint64 operate, uint64& operate_value, uint64 now_value )
    {
        if ( trigger->_trigger_type != 0u && trigger->_trigger_type != operate )
        {
            return false;
        }

        // 触发值
        if ( trigger->_trigger_check != 0u )
        {
            auto trigger_value = operate_value;
            if ( trigger->_trigger_use == KFConditionEnum::UseFinal )
            {
                trigger_value = now_value;
            }

            auto ok = KFUtility::CheckOperate<uint64>( trigger_value, trigger->_trigger_check, trigger->_trigger_value );
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
            operate_value = now_value;
            break;
        default:
            break;
        }

        return true;
    }
}

