#include "KFEntityEx.hpp"
#include "KFComponentEx.hpp"
#include "KFKernelModule.hpp"
#include "KFProtocol/KFProtocol.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    // 是否初始化完成
    bool KFEntityEx::IsInited()
    {
        return _is_inited;
    }

    void KFEntityEx::SetInited()
    {
        _is_inited = true;
    }

    bool KFEntityEx::IsNewEntity()
    {
        return _is_new;
    }

    void KFEntityEx::SetNewEntity( bool is_new )
    {
        _is_new = is_new;
    }

    void KFEntityEx::InitEntity( std::shared_ptr<KFComponentEx> component )
    {
        _is_new = false;
        _is_inited = false;
        _is_in_save = false;
        _have_sync_data = false;
        _have_show_client = false;
        _component = component;
        _show_elements.Clear();
        for ( auto i = 0u; i < __SYNC_COUNT__; ++i )
        {
            _sync_list[ i ]._type = _default_sync_sequence[ i ];
        }
    }

    DataPtr KFEntityEx::CreateData( const std::string& data_name )
    {
        auto data = Find( data_name );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return CreateData( data );
    }

    DataPtr KFEntityEx::CreateData( DataPtr data )
    {
        return KFDataFactory::Instance()->CreateData( data->_data_setting, true );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
#define __SHARED_FROM_ENTITY__ std::enable_shared_from_this<KFEntityEx>::shared_from_this()
    void KFEntityEx::UpdateData( DataPtr data, const std::string& value, bool callback )
    {
        if ( data->IsArray() )
        {
            data->Set<std::string>( value );
            for ( auto i = KFGlobal::Instance()->_array_index; i < data->Size(); ++i )
            {
                auto child = data->Find( i );
                if ( child != nullptr )
                {
                    auto new_value = child->Get<uint64>();
                    _component->UpdateDataCallBack( __SHARED_FROM_ENTITY__, i, child, i, KFEnum::Set, new_value, 0u, new_value, callback );
                }
            }
        }
        else if ( data->IsString() )
        {
            // 属性更新回调
            auto old_value = data->Get<std::string>();
            data->Set<std::string>( value );
            _component->UpdateDataCallBack( __SHARED_FROM_ENTITY__, data, old_value, value, callback );
        }
        else if ( data->IsInt() )
        {
            auto int_value = KFUtility::ToValue( value );
            UpdateData( data, KFEnum::Set, int_value, callback );
        }
    }

    void KFEntityEx::UpdateData( const std::string& data_name, const std::string& value, bool callback )
    {
        auto data = Find( data_name );
        if ( data == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, data_name );
        }

        UpdateData( data, value, callback );
    }

    void KFEntityEx::UpdateObject( DataPtr parent_data, const std::string& data_name, const std::string& value, bool callback )
    {
        auto data = parent_data->Find( data_name );
        if ( data == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", parent_data->_data_setting->_name, data_name );
        }

        UpdateData( data, value, callback );
    }

    void KFEntityEx::UpdateObject( const std::string& parent_name, const std::string& data_name, const std::string& value, bool callback )
    {
        auto parent_data = Find( parent_name );
        if ( parent_data == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parent_name );
        }

        UpdateObject( parent_data, data_name, value, callback );
    }

    void KFEntityEx::UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, const std::string& value, bool callback )
    {
        auto parent_data = Find( parent_name );
        if ( parent_data == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parent_name );
        }

        auto object_data = parent_data->Find( key );
        if ( object_data == nullptr )
        {
            return __LOG_ERROR__( "record=[{}] not have object=[{}]", parent_name, key );
        }

        UpdateObject( object_data, data_name, value, callback );
    }

    uint64 KFEntityEx::UpdateData( const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        auto data = Find( data_name );
        if ( data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, data_name );
            return _invalid_int;
        }

        return UpdateData( 0u, data, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateData( DataPtr data, uint32 operate, uint64 value, bool callback )
    {
        return UpdateData( 0u, data, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateData( uint64 key, DataPtr data, uint32 operate, uint64 value, bool callback )
    {
        if ( !data->IsInt() )
        {
            __LOG_ERROR__( "data=[{}] is not int=[{}]", data->_data_setting->_name );
            return _invalid_int;
        }

        auto old_value = data->Get<uint64>();
        auto new_value = data->Operate( operate, value );
        _component->UpdateDataCallBack( __SHARED_FROM_ENTITY__, key, data, _invalid_int, operate, value, old_value, new_value, callback );
        return new_value;
    }

    uint64 KFEntityEx::UpdateObject( const std::string& parent_name, const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        auto parent_data = Find( parent_name );
        if ( parent_data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parent_name );
            return _invalid_int;
        }

        return UpdateObject( parent_data, data_name, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateObject( DataPtr parent_data, const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        auto data = parent_data->Find( data_name );
        if ( data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", parent_data->_data_setting->_name, data_name );
            return _invalid_int;
        }

        return UpdateData( parent_data->GetKeyID(), data, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        auto parent_data = Find( parent_name );
        if ( parent_data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, parent_name );
            return _invalid_int;
        }

        return UpdateRecord( parent_data, key, data_name, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateRecord( DataPtr parent_data, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        // 不存在, 创建
        auto object_data = parent_data->Find( key );
        if ( object_data == nullptr )
        {
            object_data = CreateData( parent_data );
            value = object_data->Operate( data_name, operate, value );
            AddRecord( parent_data, key, object_data, callback );
            return value;
        }

        // 存在就更新
        auto data = object_data->Find( data_name );
        if ( data == nullptr )
        {
            __LOG_ERROR__( "object=[{}] not have data=[{}]", object_data->_data_setting->_name, data_name );
            return _invalid_int;
        }

        return UpdateData( key, data, operate, value, callback );
    }


    uint64 KFEntityEx::UpdateArray( const std::string& data_name, uint64 index, uint32 operate, uint64 value, bool callback )
    {
        auto array_data = Find( data_name );
        if ( array_data == nullptr )
        {
            __LOG_ERROR__( "object=[{}] not have array=[{}]", _data_setting->_name, data_name );
            return _invalid_int;
        }

        if ( !array_data->IsArray() )
        {
            __LOG_ERROR__( "data=[{}] is not array", data_name );
            return _invalid_int;
        }

        return UpdateArray( index, array_data, index, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateArray( DataPtr array_data, uint64 index, uint32 operate, uint64 value, bool callback )
    {
        return UpdateArray( index, array_data, index, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateArray( uint64 key, DataPtr array_data, uint64 index, uint32 operate, uint64 value, bool callback )
    {
        auto data = array_data->Find( index );
        if ( data == nullptr )
        {
            __LOG_ERROR__( "array=[{}] not index=[{}] max=[{}]", array_data->_data_setting->_name, index, array_data->MaxSize() );
            return _invalid_int;
        }

        auto old_value = data->Get<uint64>();
        auto new_value = data->Operate( operate, value );
        _component->UpdateDataCallBack( __SHARED_FROM_ENTITY__, key, data, index, operate, value, old_value, new_value, callback );
        return new_value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::AddRecord( const std::string& parent_name, uint64 key, DataPtr data, bool callback )
    {
        auto parent_data = Find( parent_name );
        if ( parent_data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parent_name );
            return false;
        }

        if ( !parent_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", parent_name );
            return false;
        }

        if ( !data->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", data->_data_setting->_name );
            return false;
        }

        return AddRecord( parent_data, key, data, callback );
    }

    bool KFEntityEx::AddRecord( DataPtr parent_data, DataPtr data, bool callback )
    {
        if ( !parent_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", parent_data->_data_setting->_name );
            return false;
        }

        if ( !data->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", data->_data_setting->_name );
            return false;
        }

        auto key = data->GetKeyID();
        return AddRecord( parent_data, key, data, callback );
    }

    bool KFEntityEx::AddRecord( DataPtr parent_data, uint64 key, DataPtr data, bool callback )
    {
        bool result = parent_data->Add( key, data );
        if ( result )
        {
            // 添加属性回调
            _component->AddDataCallBack( __SHARED_FROM_ENTITY__, parent_data, key, data, callback );
        }

        return result;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::InitArray( DataPtr array_data, uint32 size )
    {
        KFDataFactory::Instance()->InitArray( array_data, size );
    }

    DataPtr KFEntityEx::AddArray( DataPtr array_data, int64 value )
    {
        auto data = KFDataFactory::Instance()->AddArray( array_data );
        data->Set( value );
        return data;
    }

#define __ADD_ARRAY_DATA__( type )\
    void KFEntityEx::AddArray( DataPtr data, const type& inlist )\
    {\
        if ( data->IsArray() )\
        {\
            return __LOG_ERROR__( "data=[{}] is not array", data->_data_setting->_name );\
        }\
        for ( auto value : inlist )\
        {\
            AddArray( data, value );\
        }\
    }\

#define __ADD_ARRAY_DATA_1__( type )\
    void KFEntityEx::AddObjectArray( DataPtr data, const std::string& data_name, const type& inlist )\
    {\
        if ( data->IsObject() )\
        {\
            return __LOG_ERROR__( "data=[{}] is not object", data->_data_setting->_name );\
        }\
        auto array_data = data->Find( data_name );\
        if ( array_data == nullptr )\
        {\
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", data->_data_setting->_name, data_name );\
        }\
        AddArray( array_data, inlist );\
    }\

    __ADD_ARRAY_DATA__( UInt32Set );
    __ADD_ARRAY_DATA__( UInt32List );
    __ADD_ARRAY_DATA__( UInt32Vector );

    __ADD_ARRAY_DATA_1__( UInt32Set );
    __ADD_ARRAY_DATA_1__( UInt32List );
    __ADD_ARRAY_DATA_1__( UInt32Vector );
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::RemoveRecord( const std::string& parent_name, uint64 key, bool callback )
    {
        auto parent_data = Find( parent_name );
        if ( parent_data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parent_name );
            return false;
        }

        return RemoveRecord( parent_data, key, callback );
    }

    bool KFEntityEx::RemoveRecord( DataPtr parent_data, uint64 key, bool callback )
    {
        if ( !parent_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", parent_data->_data_setting->_name );
            return false;
        }

        auto data = parent_data->Remove( key );
        if ( data == nullptr )
        {
            __LOG_ERROR__( "record=[{}] not have object=[{}]", parent_data->_data_setting->_name, key );
            return false;
        }

        _component->RemoveDataCallBack( __SHARED_FROM_ENTITY__, parent_data, key, data, callback );
        KFDataFactory::Instance()->DestroyData( data );
        return true;
    }

    bool KFEntityEx::ClearRecord( const std::string& parent_name, bool callback )
    {
        auto parent_data = Find( parent_name );
        if ( parent_data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parent_name );
            return false;
        }

        return ClearRecord( parent_data, callback );
    }

    bool KFEntityEx::ClearRecord( DataPtr parent_data, bool callback )
    {
        if ( !parent_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", parent_data->_data_setting->_name );
            return false;
        }

        UInt64List keys;
        for ( auto data = parent_data->First(); data != nullptr; data = parent_data->Next() )
        {
            keys.push_back( data->GetKeyID() );
        }

        for ( auto key : keys )
        {
            RemoveRecord( parent_data, key, callback );
        }

        return true;
    }

    bool KFEntityEx::RemoveObject( DataPtr parent_data, const std::string& data_name, bool callback )
    {
        if ( !parent_data->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", parent_data->_data_setting->_name );
            return false;
        }

        auto data = parent_data->Remove( data_name );
        if ( data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", parent_data->_data_setting->_name, data_name );
            return false;
        }

        // 先移除属性
        _component->RemoveDataCallBack( __SHARED_FROM_ENTITY__, parent_data, 0, data, callback );
        KFDataFactory::Instance()->DestroyData( data );
        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFEntityEx::MoveRecord( DataPtr parent_data, uint64 key, bool callback )
    {
        if ( !parent_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", parent_data->_data_setting->_name );
            return nullptr;
        }

        // 移除属性
        auto data = parent_data->Remove( key );
        if ( data != nullptr )
        {
            _component->RemoveDataCallBack( __SHARED_FROM_ENTITY__, parent_data, key, data, callback );
        }
        else
        {
            __LOG_WARN__( "record=[{}] not have object=[{}]", parent_data->_data_setting->_name, key );
        }

        return data;
    }


    DataPtr KFEntityEx::MoveRecordToRecord( const std::string& source_name, uint64 key, const std::string& target_name )
    {
        auto source_data = Find( source_name );
        auto target_data = Find( target_name );
        if ( source_data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, source_name );
            return nullptr;
        }

        if ( target_data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, target_name );
            return nullptr;
        }

        return MoveRecordToRecord( source_data, key, target_data );
    }

    DataPtr KFEntityEx::MoveRecordToRecord( DataPtr source_data, uint64 key, DataPtr target_data )
    {
        if ( !source_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", source_data->_data_setting->_name );
            return nullptr;
        }

        if ( !target_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", target_data->_data_setting->_name );
            return nullptr;
        }


        auto data = source_data->Remove( key );
        if ( data != nullptr )
        {
            _component->RemoveDataCallBack( __SHARED_FROM_ENTITY__, source_data, key, data, false );

            target_data->Add( key, data );
            _component->AddDataCallBack( __SHARED_FROM_ENTITY__, target_data, key, data, false );
        }
        else
        {
            __LOG_ERROR__( "record=[{}] not have object=[{}]", source_data->_data_setting->_name, key );
        }

        return data;
    }

    DataPtr KFEntityEx::MoveObjectToRecord( DataPtr source_data, const std::string& data_name, const std::string& target_name )
    {
        auto target_data = Find( target_name );
        if ( target_data == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, target_name );
            return nullptr;
        }

        return MoveObjectToRecord( source_data, data_name, target_data );
    }

    DataPtr KFEntityEx::MoveObjectToRecord( DataPtr source_data, const std::string& data_name, DataPtr target_data )
    {
        if ( !source_data->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", source_data->_data_setting->_name );
            return nullptr;
        }

        if ( !target_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", target_data->_data_setting->_name );
            return nullptr;
        }

        // 移除属性
        auto data = source_data->Remove( data_name, true );
        if ( data != nullptr )
        {
            _component->RemoveDataCallBack( __SHARED_FROM_ENTITY__, source_data, 0, data, false );

            // 添加属性
            auto uuid = data->GetKeyID();
            target_data->Add( uuid, data );
            _component->AddDataCallBack( __SHARED_FROM_ENTITY__, target_data, uuid, data, false );
        }
        else
        {
            __LOG_ERROR__( "object=[{}] not have data=[{}]", source_data->_data_setting->_name, data_name );
        }

        return data;
    }

    DataPtr KFEntityEx::MoveRecordToObject( DataPtr source_data, uint64 key, DataPtr target_data, const std::string& data_name )
    {
        if ( !source_data->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", source_data->_data_setting->_name );
            return nullptr;
        }

        if ( !target_data->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", target_data->_data_setting->_name );
            return nullptr;
        }

        // 移除属性
        auto data = source_data->Remove( key );
        if ( data != nullptr )
        {
            _component->RemoveDataCallBack( __SHARED_FROM_ENTITY__, source_data, key, data, false );

            // 添加属性
            target_data->Add( data_name, data );
            SyncUpdateDataToClient( data, key );
        }
        else
        {
            __LOG_ERROR__( "record=[{}] not have object=[{}]", source_data->_data_setting->_name, key );
        }

        return data;
    }

    DataPtr KFEntityEx::MoveObjectToObject( DataPtr source_data, const std::string& source_name, DataPtr target_data, const std::string& target_name )
    {
        if ( !source_data->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", source_data->_data_setting->_name );
            return nullptr;
        }


        if ( !target_data->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", target_data->_data_setting->_name );
            return nullptr;
        }

        // 移除属性
        auto data = source_data->Remove( source_name, true );
        if ( data != nullptr )
        {
            _component->RemoveDataCallBack( __SHARED_FROM_ENTITY__, source_data, 0, data, false );

            // 添加属性
            target_data->Add( target_name, data );
            SyncUpdateDataToClient( data, 0u );
        }
        else
        {
            __LOG_ERROR__( "object=[{}] not have object=[{}]", source_data->_data_setting->_name, source_name );
        }

        return data;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::LogElementResult( const KFElementResult* result, const char* function, uint32 line )
    {
        // 打印日志
        switch ( result->_show_type )
        {
        case KFDataShowEnum::Show_Element:
            LogElementResultElement( result, function, line );
            break;
        case KFDataShowEnum::Show_Currency:
            LogElementResultCurrency( result, function, line );
            break;
        case KFDataShowEnum::Show_Object:
            LogElementResultObject( result, function, line );
            break;
        case KFDataShowEnum::Show_Overlay:
            LogElementResultOverlay( result, function, line );
            break;
        case KFDataShowEnum::Show_NotOverlay:
            LogElementResultNotOverlay( result, function, line );
            break;
        default:
            break;
        }
    }

    void KFEntityEx::ShowElementResult( const KFElementResult* result )
    {
        auto ok = _component->CallElementResultFunction( __SHARED_FROM_ENTITY__, result );
        if ( !ok )
        {
            return;
        }

        switch ( result->_show_type )
        {
        case KFDataShowEnum::Show_Element:
            ShowElementResultElement( result );
            break;
        case KFDataShowEnum::Show_Currency:
            ShowElementResultCurrency( result );
            break;
        case KFDataShowEnum::Show_Object:
            ShowElementResultObject( result );
            break;
        case KFDataShowEnum::Show_Overlay:
            ShowElementResultOverlay( result );
            break;
        case KFDataShowEnum::Show_NotOverlay:
            ShowElementResultNotOverlay( result );
            break;
        default:
            break;
        }
    }

    KFMsg::PBShowElement* KFEntityEx::CreateShowElement()
    {
        auto size = _show_elements.element_size();
        if ( size <= 0 )
        {
            return nullptr;
        }

        return _show_elements.mutable_element( size - 1 );
    }

    KFMsg::PBShowElement* KFEntityEx::CreateShowElement( const std::string& module_name, uint64 module_id )
    {
        for ( auto i = 0; i < _show_elements.element_size(); ++i )
        {
            auto show_element = _show_elements.mutable_element( i );
            if ( show_element->modulename() == module_name &&
                    show_element->moduleid() == module_id )
            {
                return show_element;
            }
        }

        _have_show_client = true;
        _component->AddSyncEntity( __SHARED_FROM_ENTITY__ );

        auto show_element = _show_elements.add_element();
        show_element->set_modulename( module_name );
        show_element->set_moduleid( module_id );
        return show_element;
    }

    KFMsg::PBShowData* KFEntityEx::CreateShowData( KFMsg::PBShowElement* show_element, const std::string& name, uint64 value, bool is_merge )
    {
        if ( is_merge )
        {
            for ( auto i = 0; i < show_element->pbdata_size(); ++i )
            {
                auto show_data = show_element->mutable_pbdata( i );
                if ( show_data->name() != name )
                {
                    continue;
                }

                auto data_setting = _component->_data_setting->_class_setting->FindSetting( name );
                if ( data_setting == nullptr )
                {
                    continue;
                }

                // 如果是record/object 判断value是否相同
                if ( data_setting->_type == KFDataDefine::DataTypeRecord || data_setting->_type == KFDataDefine::DataTypeObject )
                {
                    if ( show_data->value() != value )
                    {
                        continue;
                    }
                }
                else
                {
                    show_data->set_value( show_data->value() + value );
                }

                return show_data;
            }
        }

        auto show_data = show_element->add_pbdata();
        show_data->set_name( name );
        show_data->set_value( value );
        return show_data;
    }

    bool KFEntityEx::IsElementResultShow( const KFElementResult* result )
    {
        if ( !IsInited() || !result->_is_need_show || result->_module_name.empty() )
        {
            return false;
        }

        return true;
    }

    void KFEntityEx::LogElementResultElement( const KFElementResult* result, const char* function, uint32 line )
    {
        if ( result->_operate == KFEnum::Add )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] add=[{}]", _component->_name, GetKeyID(), result->_element->ToString() );
        }
        else
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] remove=[{}]", _component->_name, GetKeyID(), result->_element->ToString() );
        }
    }

    void KFEntityEx::ShowElementResultElement( const KFElementResult* result )
    {
        if ( !IsElementResultShow( result ) )
        {
            return;
        }

        auto show_element = CreateShowElement( result->_module_name, result->_module_id );

        if ( result->_element->IsValue() )
        {
            auto element_value = std::static_pointer_cast<const KFElementValue>( result->_element );
            if ( element_value->_value->IsNeedShow() )
            {
                CreateShowData( show_element, element_value->_data_name, element_value->_value->GetUseValue(), false );
            }
        }
        else if ( result->_element->IsObject() )
        {
            auto element_object = std::static_pointer_cast<const KFElementObject>( result->_element );
            if ( element_object->IsNeedShow() )
            {
                auto show_data = CreateShowData( show_element, element_object->_data_name, element_object->_config_id, false );
                ( *show_data->mutable_pbuint64() )[ __STRING__( id ) ] = element_object->_config_id;
                for ( auto& iter : element_object->_values._objects )
                {
                    auto value_data = iter.second;
                    if ( value_data->IsNeedShow() )
                    {
                        ( *show_data->mutable_pbuint64() )[ iter.first ] = value_data->GetUseValue();
                    }
                }
            }
        }
    }

    void KFEntityEx::LogElementResultCurrency( const KFElementResult* result, const char* function, uint32 line )
    {
        if ( result->_operate == KFEnum::Add )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] add=[{{\"{}\":{}}}]",
                                   _component->_name, GetKeyID(), result->_element->_data_name, result->_total_value );
        }
        else
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] remove=[{{\"{}\":{}}}]",
                                   _component->_name, GetKeyID(), result->_element->_data_name, result->_total_value );
        }
    }


    void KFEntityEx::ShowElementResultCurrency( const KFElementResult* result )
    {
        if ( IsElementResultShow( result ) )
        {
            AddDataToShow( result->_module_name, result->_module_id, result->_element->_data_name, result->_total_value, true );
        }
    }

    void KFEntityEx::LogElementResultObject( const KFElementResult* result, const char* function, uint32 line )
    {
        if ( result->_operate == KFEnum::Add )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] add=[{{\"{}\":{{\"{}\":{}}}}}]",
                                   _component->_name, GetKeyID(), result->_element->_data_name, result->_data_name, result->_total_value );
        }
        else
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] remove=[{{\"{}\":{{\"{}\":{}}}}}]",
                                   _component->_name, GetKeyID(), result->_element->_data_name, result->_data_name, result->_total_value );
        }
    }

    void KFEntityEx::ShowElementResultObject( const KFElementResult* result )
    {
        if ( IsElementResultShow( result ) )
        {
            AddDataToShow( result->_module_name, result->_module_id, result->_element->_data_name, result->_total_value, true );
        }
    }

    void KFEntityEx::LogElementResultOverlay( const KFElementResult* result, const char* function, uint32 line )
    {
        for ( auto& iter : result->_overlay_list )
        {
            if ( result->_operate == KFEnum::Add )
            {
                __LOG_INFO_FUNCTION__( function, line, "{}=[{}] extend=[{}] add=[{{\"{}\":{{\"{}\":{},\"{}\":{}}}}}]",
                                       _component->_name, GetKeyID(), iter.first,
                                       result->_element->_data_name, __STRING__( id ), result->_config_id, result->_data_name, iter.second );
            }
            else
            {
                __LOG_INFO_FUNCTION__( function, line, "{}=[{}] extend=[{}] remove=[{{\"{}\":{{\"{}\":{},\"{}\":{}}}}}]",
                                       _component->_name, GetKeyID(), iter.first,
                                       result->_element->_data_name, __STRING__( id ), result->_config_id, result->_data_name, iter.second );
            }
        }
    }

    void KFEntityEx::ShowElementResultOverlay( const KFElementResult* result )
    {
        for ( auto& iter : result->_overlay_list )
        {
            if ( IsElementResultShow( result ) )
            {
                StringUInt64 values;
                values[result->_data_name] = iter.second;
                values[__STRING__( id )] = result->_config_id;
                AddDataToShow( result->_module_name, result->_module_id, result->_element->_data_name, result->_config_id, values, true );
            }
        }
    }

    void KFEntityEx::LogElementResultNotOverlay( const KFElementResult* result, const char* function, uint32 line )
    {
        for ( auto data : result->_not_overlay_list )
        {
            if ( result->_operate == KFEnum::Add )
            {
                __LOG_INFO_FUNCTION__( function, line, "{}=[{}] extend=[{}] add=[{{\"{}\":{}}}]",
                                       _component->_name, GetKeyID(), data->_data_setting->_name,
                                       result->_element->_data_name, data->ToString() );
            }
            else
            {
                __LOG_INFO_FUNCTION__( function, line, "{}=[{}] extend=[{}] remove=[{{\"{}\":{}}}]",
                                       _component->_name, GetKeyID(), data->_data_setting->_name,
                                       result->_element->_data_name, data->ToString() );
            }
        }
    }


    void KFEntityEx::ShowElementResultNotOverlay( const KFElementResult* result )
    {
        for ( auto data : result->_not_overlay_list )
        {
            if ( IsElementResultShow( result ) )
            {
                AddDataToShow( result->_module_name, result->_module_id, data );
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::SetDataShowModule( const std::string& module_name, uint64 module_id )
    {
        if ( module_name.empty() )
        {
            return;
        }

        CreateShowElement( module_name, module_id );
    }

    void KFEntityEx::AddDataToShow( const std::string& module_name, uint64 module_id, DataPtr data, bool is_merge )
    {
        auto show_element = CreateShowElement( module_name, module_id );
        AddDataToShowElement( show_element, data, is_merge );
    }

    void KFEntityEx::AddDataToShow( const std::string& name, uint64 value, bool is_merge )
    {
        auto show_element = CreateShowElement();
        if ( show_element == nullptr )
        {
            return;
        }

        CreateShowData( show_element, name, value, is_merge );
    }

    void KFEntityEx::AddDataToShow( const std::string& module_name, uint64 module_id, const std::string& name, uint64 value, bool is_merge )
    {
        if ( value == 0u )
        {
            return;
        }

        auto show_element = CreateShowElement( module_name, module_id );
        CreateShowData( show_element, name, value, is_merge );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddDataToShow( const std::string& name, uint64 value, StringUInt64& values, bool is_merge )
    {
        auto show_element = CreateShowElement();
        if ( show_element == nullptr )
        {
            return;
        }

        AddDataToShowElement( show_element, name, value, values, is_merge );
    }

    void KFEntityEx::AddDataToShow( const std::string& module_name, uint64 module_id, const std::string& name, uint64 value, StringUInt64& values, bool is_merge )
    {
        auto show_element = CreateShowElement( module_name, module_id );
        AddDataToShowElement( show_element, name, value, values, is_merge );
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddDataToShowElement( KFMsg::PBShowElement* show_element, DataPtr data, bool is_merge )
    {
        if ( !data->_data_setting->HaveMask( KFDataDefine::DataMaskShow ) )
        {
            return;
        }

        switch ( data->_data_type )
        {
        case KFDataDefine::DataTypeInt32:
        case KFDataDefine::DataTypeUInt32:
        case KFDataDefine::DataTypeInt64:
        case KFDataDefine::DataTypeUInt64:
        {
            CreateShowData( show_element, data->_data_setting->_logic_name, data->Get(), is_merge );
            break;
        }
        case KFDataDefine::DataTypeObject:
        case KFDataDefine::DataTypeRecord:
        {
            auto config_id = data->Get( data->_data_setting->_config_key_name );
            auto show_data = CreateShowData( show_element, data->_data_setting->_logic_name, config_id, false );
            for ( auto child = data->First(); child != nullptr; child = data->Next() )
            {
                if ( !child->_data_setting->HaveMask( KFDataDefine::DataMaskShow ) ||
                        !child->IsValid() )
                {
                    continue;
                }

                ( *show_data->mutable_pbuint64() )[child->_data_setting->_logic_name] = child->Get();
            }
            break;
        }
        }
    }

    void KFEntityEx::AddDataToShowElement( KFMsg::PBShowElement* show_element, const std::string& name, uint64 value, StringUInt64& values, bool is_merge )
    {
        auto show_data = CreateShowData( show_element, name, value, is_merge );
        for ( auto& iter : values )
        {
            if ( iter.first == __STRING__( count ) )
            {
                ( *show_data->mutable_pbuint64() )[ iter.first ] += iter.second;
            }
            else
            {
                ( *show_data->mutable_pbuint64() )[ iter.first ] = iter.second;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFEntityEx::FindByLogicName( const std::string& logic_name )
    {
        auto data_setting = _data_setting->_class_setting->FindSettingByLogicName( logic_name );
        if ( data_setting == nullptr )
        {
            return nullptr;
        }

        return Find( data_setting->_name );
    }

    const std::string& KFEntityEx::CheckAddElement( const KFElements* elements, double multiple, const char* function, uint32 line )
    {
        for ( auto element : elements->_element_list )
        {
            auto ok = CheckAddElement( element, multiple, function, line );
            if ( !ok )
            {
                return element->_data_name;
            }
        }

        return _invalid_string;
    }

    bool KFEntityEx::CheckAddElement( std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line )
    {
        auto data = Find( element->_data_name );
        if ( data == nullptr )
        {
            data = FindByLogicName( element->_data_name );
            if ( data == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "can't find data=[{}]", element->_data_name );
                return false;
            }
        }

        // 如果有注册函数, 执行注册函数
        auto function_data = _component->_check_add_element_function.Find( data->_data_setting->_logic_name );
        if ( function_data != nullptr )
        {
            return function_data->CallEx<bool>( __SHARED_FROM_ENTITY__, data, element, multiple, function, line );
        }

        // 找不到处理函数, 用基础函数来处理
        return !data->IsFull();
    }

    void KFEntityEx::AddElement( const KFElements* elements, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        if ( elements->_element_list.empty() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "{}=[{}] element=[{}] is empty", _component->_name, GetKeyID(), elements->GetElement() );
        }

        __LOG_INFO_FUNCTION__( function, line, "{}=[{}] start add elements={} multiple=[{:0.2f}]", _component->_name, GetKeyID(), elements->GetElement(), multiple );
        _element_seq = KFGlobal::Instance()->STMakeUuid();
        for ( auto element : elements->_element_list )
        {
            AddElement( element, multiple, module_name, module_id, function, line );
        }
        __LOG_INFO_FUNCTION__( function, line, "{}=[{}] finish add elements", _component->_name, GetKeyID() );
    }

    // 添加元数据
    void KFEntityEx::AddElement( std::shared_ptr<KFElement> element, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        auto data = Find( element->_data_name );
        if ( data == nullptr )
        {
            data = FindByLogicName( element->_data_name );
            if ( data == nullptr )
            {
                return __LOG_ERROR_FUNCTION__( function, line, "can't find data=[{}]", element->_data_name );
            }
        }

        element->_data_setting = data->_data_setting;

        KFElementResult element_result;
        element_result._element = element;
        element_result._multiple = multiple;
        element_result._operate = KFEnum::Add;
        element_result._sequence = _element_seq;
        element_result._module_name = module_name;
        element_result._module_id = module_id;
        element_result._is_need_show = data->_data_setting->HaveMask( KFDataDefine::DataMaskShow );

        auto ok = false;
        auto function_data = _component->_add_element_function.Find( data->_data_setting->_logic_name );
        if ( function_data != nullptr )
        {
            ok = function_data->CallEx<bool>( __SHARED_FROM_ENTITY__, data, &element_result, function, line );
        }
        else
        {
            // 没有注册的函数
            switch ( data->_data_type )
            {
            case KFDataDefine::DataTypeObject:
                ok = AddObjectElement( data, &element_result, function, line );
                break;
            case KFDataDefine::DataTypeRecord:
                ok = AddRecordElement( data, &element_result, function, line );
                break;
            default:
                ok = AddNormalElement( data, &element_result, function, line );
                break;
            }
        }

        if ( ok )
        {
            // 显示属性
            ShowElementResult( &element_result );

            // 打印日志
            LogElementResult( &element_result, function, line );

            // 运行日志
            _component->CallLogElementFunction( __SHARED_FROM_ENTITY__, &element_result );
        }
    }

    bool KFEntityEx::AddNormalElement( DataPtr data, KFElementResult* result, const char* function, uint32 line )
    {
        if ( !result->_element->IsValue() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value", result->_element->_data_name );
            return false;
        }

        auto element_value = std::static_pointer_cast<KFElementValue>( result->_element );
        switch ( element_value->_value->_type )
        {
        case KFDataDefine::DataTypeUInt32:
        {
            auto use_value = element_value->_value->CalcUseValue( data->_data_setting, result->_multiple );
            UpdateData( data, element_value->_operate, use_value );

            result->AddResult( use_value );
        }
        break;
        case KFDataDefine::DataTypeString:
            UpdateData( data, element_value->_value->GetValue() );
            break;
        default:
            break;
        }

        return true;
    }

    void KFEntityEx::UpdateElementToData( DataPtr data, std::shared_ptr<KFElementObject> element, double multiple )
    {
        for ( auto& iter : element->_values._objects )
        {
            auto child = data->Find( iter.first );
            if ( child == nullptr ||
                    child->_data_setting->_name == data->_data_setting->_key_name ||
                    child->_data_setting->_name == data->_data_setting->_config_key_name )
            {
                continue;
            }

            auto element_value = iter.second;
            switch ( element_value->_type )
            {
            case KFDataDefine::DataTypeUInt32:
                UpdateData( child, element->_operate, element_value->CalcUseValue( child->_data_setting, multiple ) );
                break;
            case KFDataDefine::DataTypeString:
                UpdateData( child, element_value->GetValue() );
                break;
            case KFDataDefine::DataTypeObject:
            {
                auto object_value = std::static_pointer_cast<KFObjValue>( element_value );
                object_value->_element->SetOperate( element->_operate );
                UpdateElementToData( child, object_value->_element, multiple );
            }
            break;
            default:
                break;
            }
        }
    }

    void KFEntityEx::SetElementToData( DataPtr data, std::shared_ptr<KFElementObject> element, double multiple )
    {
        for ( auto& iter : element->_values._objects )
        {
            auto child = data->Find( iter.first );
            if ( child == nullptr )
            {
                continue;
            }

            auto element_value = iter.second;
            switch ( element_value->_type )
            {
            case KFDataDefine::DataTypeUInt32:
                child->Set( element_value->CalcUseValue( child->_data_setting, multiple ) );
                break;
            case KFDataDefine::DataTypeString:
                child->Set( element_value->GetValue() );
                break;
            case KFDataDefine::DataTypeObject:
                SetElementToData( child, std::static_pointer_cast<KFObjValue>( element_value )->_element, multiple );
                break;
            default:
                break;
            }
        }
    }

    bool KFEntityEx::AddObjectElement( DataPtr parent_data, KFElementResult* result, const char* function, uint32 line )
    {
        if ( !result->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", result->_element->_data_name );
            return false;
        }

        UpdateElementToData( parent_data, std::static_pointer_cast<KFElementObject>( result->_element ), result->_multiple );
        return true;
    }

    bool KFEntityEx::AddRecordElement( DataPtr parent_data, KFElementResult* result, const char* function, uint32 line )
    {
        if ( !result->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", result->_element->_data_name );
            return false;
        }

        auto element_object = std::static_pointer_cast<KFElementObject>( result->_element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", result->_element->_data_name );
            return false;
        }

        auto child = parent_data->Find( element_object->_config_id );
        if ( child == nullptr )
        {
            // 不存在, 创建一个, 并更新属性
            child = CreateData( parent_data );
            SetElementToData( child, element_object, result->_multiple );
            AddRecord( parent_data, element_object->_config_id, child );
        }
        else
        {
            // 存在直接更新属性
            UpdateElementToData( child, element_object, result->_multiple );
        }

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 判断元数据是否满足条件
    const std::string& KFEntityEx::CheckRemoveElement( const KFElements* elements, double multiple, const char* function, uint32 line )
    {
        for ( auto element : elements->_element_list )
        {
            auto ok = CheckRemoveElement( element, multiple, function, line  );
            if ( !ok )
            {
                return element->_data_name;
            }
        }

        return _invalid_string;
    }

    bool KFEntityEx::CheckRemoveElement( std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line )
    {
        auto data = Find( element->_data_name );
        if ( data == nullptr )
        {
            data = FindByLogicName( element->_data_name );
            if ( data == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "can't find data=[{}]", element->_data_name );
                return false;
            }
        }

        // 如果有注册函数, 执行注册函数
        auto function_data = _component->_check_remove_element_function.Find( data->_data_setting->_logic_name );
        if ( function_data != nullptr )
        {
            return function_data->CallEx<bool>( __SHARED_FROM_ENTITY__, data, element, multiple, function, line );
        }

        // 找不到处理函数, 用基础函数来处理
        switch ( data->_data_type )
        {
        case KFDataDefine::DataTypeRecord:
            return CheckRecordElement( data, element, multiple, function, line );
            break;
        case KFDataDefine::DataTypeObject:
            return CheckObjectElement( data, element, multiple, function, line );
            break;
        default:
            return CheckNormalElement( data, element, multiple, function, line );
            break;
        }

        return false;
    }

    bool KFEntityEx::CheckNormalElement( DataPtr data, std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line )
    {
        if ( !element->IsValue() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value", element->_data_name );
            return false;
        }

        auto element_value = std::static_pointer_cast<KFElementValue>( element );
        if ( !element_value->_value->IsType( KFDataDefine::DataTypeUInt32 ) )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", element->_data_name );
            return false;
        }

        auto data_value = data->Get();
        auto use_value = element_value->_value->CalcUseValue( data->_data_setting, multiple );
        return data_value >= use_value;
    }

    bool KFEntityEx::CheckObjectElement( DataPtr parent_data, std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line )
    {
        if ( !element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", element->_data_name );
            return false;
        }

        auto element_object = std::static_pointer_cast<KFElementObject>( element );
        for ( auto& iter : element_object->_values._objects )
        {
            auto& name = iter.first;
            auto element_value = iter.second;
            if ( !element_value->IsType( KFDataDefine::DataTypeUInt32 ) )
            {
                __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", name );
                return false;
            }

            auto child = parent_data->Find( name );
            if ( child == nullptr )
            {
                return false;
            }

            auto data_value = child->Get() ;
            auto use_value = element_value->CalcUseValue( child->_data_setting, multiple );
            if ( data_value < use_value )
            {
                return false;
            }
        }

        return true;
    }

    bool KFEntityEx::CheckRecordElement( DataPtr parent_data, std::shared_ptr<KFElement> element, double multiple, const char* function, uint32 line )
    {
        if ( !element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", element->_data_name );
            return false;
        }

        auto element_object = std::static_pointer_cast<KFElementObject>( element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", element->_data_name );
            return false;
        }

        auto data = parent_data->Find( element_object->_config_id );
        if ( data == nullptr )
        {
            return false;
        }

        return CheckObjectElement( data, element, multiple, function, line );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    const std::string& KFEntityEx::RemoveElement( const KFElements* elements, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        if ( elements->IsEmpty() )
        {
            return _invalid_string;
        }

        auto& data_name = CheckRemoveElement( elements, multiple, function, line );
        if ( data_name.empty() )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] start remove elements={} multiple=[{:0.2f}]", _component->_name, GetKeyID(), elements->GetElement(), multiple );
            _element_seq = KFGlobal::Instance()->STMakeUuid();
            for ( auto element : elements->_element_list )
            {
                RemoveElement( element, multiple, module_name, module_id, function, line );
            }
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] finish remove elements", _component->_name, GetKeyID() );
        }

        return data_name;
    }

    // 删除元数据
    void KFEntityEx::RemoveElement( std::shared_ptr<KFElement> element, double multiple, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        auto data = Find( element->_data_name );
        if ( data == nullptr )
        {
            data = FindByLogicName( element->_data_name );
            if ( data == nullptr )
            {
                return __LOG_ERROR_FUNCTION__( function, line, "can't find data=[{}]", element->_data_name );
            }
        }

        KFElementResult element_result;
        element_result._element = element;
        element_result._multiple = multiple;
        element_result._operate = KFEnum::Dec;
        element_result._sequence = _element_seq;
        element_result._module_name = module_name;
        element_result._module_id = module_id;

        auto ok = false;
        auto function_data = _component->_remove_element_function.Find( data->_data_setting->_logic_name );
        if ( function_data != nullptr )
        {
            ok = function_data->CallEx<bool>( __SHARED_FROM_ENTITY__, data, &element_result, function, line );
        }
        else
        {
            // 找不到处理函数, 用基础函数来处理
            switch ( data->_data_type )
            {
            case KFDataDefine::DataTypeRecord:
                ok = RemoveRecordElement( data, &element_result, function, line );
                break;
            case KFDataDefine::DataTypeObject:
                ok = RemoveObjectElement( data, &element_result, function, line );
                break;
            default:
                ok = RemoveNormalElement( data, &element_result, function, line );
                break;
            }
        }

        if ( ok )
        {
            // 打印日志
            LogElementResult( &element_result, function, line );

            // 运营日志
            _component->CallLogElementFunction( __SHARED_FROM_ENTITY__, &element_result );
        }
    }

    bool KFEntityEx::RemoveNormalElement( DataPtr data, KFElementResult* result, const char* function, uint32 line )
    {
        if ( !result->_element->IsValue() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value", result->_element->_data_name );
            return false;
        }

        auto element_value = std::static_pointer_cast<KFElementValue>( result->_element );
        if ( !element_value->_value->IsType( KFDataDefine::DataTypeUInt32 ) )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", result->_element->_data_name );
            return false;
        }

        auto use_value = element_value->_value->GetUseValue();
        UpdateData( data, KFEnum::Dec, use_value );

        // 添加结果
        result->AddResult( use_value );
        return true;
    }

    bool KFEntityEx::RemoveObjectElement( DataPtr object_data, KFElementResult* result, const char* function, uint32 line )
    {
        if ( !result->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", result->_element->_data_name );
            return false;
        }

        auto element_object = std::static_pointer_cast<KFElementObject>( result->_element );
        for ( auto& iter : element_object->_values._objects )
        {
            auto& name = iter.first;
            auto element_value = iter.second;
            if ( !element_value->IsType( KFDataDefine::DataTypeUInt32 ) )
            {
                __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", name );
                continue;
            }

            UpdateObject( object_data, name, KFEnum::Dec, element_value->GetUseValue() );
        }

        return true;
    }

    bool KFEntityEx::RemoveRecordElement( DataPtr parent_data, KFElementResult* result, const char* function, uint32 line )
    {
        if ( !result->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", result->_element->_data_name );
            return false;
        }

        auto element_object = std::static_pointer_cast<KFElementObject>( result->_element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", result->_element->_data_name );
            return false;
        }

        auto object_data = parent_data->Find( element_object->_config_id );
        if ( object_data == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] can't find id=[{}]", element_object->_data_name, element_object->_config_id );
            return false;
        }

        for ( auto& iter : element_object->_values._objects )
        {
            auto& name = iter.first;
            auto element_value = iter.second;
            if ( !element_value->IsType( KFDataDefine::DataTypeUInt32 ) )
            {
                __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", name );
                continue;
            }

            UpdateObject( object_data, name, KFEnum::Dec, element_value->GetUseValue() );
        }

        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
#define __PREPARE_SYNC__( function )\
    if ( !function.IsOpen() ||\
            !IsInited() || !data->IsNeedSyncToClient() )\
    {\
        return;\
    }\
    std::list<DataPtr> data_hierarchy;\
    data_hierarchy.push_front( data );\
    auto parent_data = data->GetParent();\
    while ( parent_data != nullptr )\
    {\
        if ( !parent_data->IsNeedSyncToClient() )\
        {\
            return;\
        }\
        data_hierarchy.push_front( parent_data->GetDataPtr() );\
        parent_data = parent_data->GetParent();\
    }\
    data_hierarchy.pop_front();\
    if ( data_hierarchy.empty() )\
    {\
        return;\
    }\

#define __FIND_PROTO_OBJECT__\
    auto savedata = data_hierarchy.front();\
    data_hierarchy.pop_front();\
    if ( savedata->_data_type == KFDataDefine::DataTypeObject )\
    {\
        proto_object = &( ( *proto_object->mutable_pbobject() )[savedata->_data_setting->_name] );\
    }\
    else if ( savedata->_data_type == KFDataDefine::DataTypeRecord )\
    {\
        auto pbrecord = &( ( *proto_object->mutable_pbrecord() )[savedata->_data_setting->_name] );\
        savedata = data_hierarchy.front();\
        data_hierarchy.pop_front(); \
        proto_object = &( ( *pbrecord->mutable_pbobject() )[savedata->GetKeyID()] ); \
    }\

    void KFEntityEx::SyncAddDataToClient( DataPtr data, uint64 key )
    {
        __PREPARE_SYNC__( _component->_entity_sync_add_function );
        auto proto_object = CreateSyncPBObject( KFEnum::Add );
        do
        {
            __FIND_PROTO_OBJECT__;
            if ( savedata == data )
            {
                static_cast<KFKernelModule*>( _kf_kernel )->SaveToObject( savedata, proto_object, KFDataDefine::DataMaskSync );
            }
        } while ( !data_hierarchy.empty() );
    }

    void KFEntityEx::SyncRemoveDataToClient( DataPtr data, uint64 key )
    {
        __PREPARE_SYNC__( _component->_entity_sync_remove_function );
        auto proto_object = CreateSyncPBObject( KFEnum::Dec );
        do
        {
            __FIND_PROTO_OBJECT__;
        } while ( !data_hierarchy.empty() );
    }

    void KFEntityEx::SynAddRecordDataToClient( DataPtr data )
    {
        for ( auto child = data->First(); child != nullptr; child = data->Next() )
        {
            SyncAddDataToClient( child, child->GetKeyID() );
        }
    }

    void KFEntityEx::SyncUpdateDataToClient( DataPtr data, uint64 key )
    {
        __PREPARE_SYNC__( _component->_entity_sync_update_function );
        auto proto_object = CreateSyncPBObject( KFEnum::Set );
        do
        {
            __FIND_PROTO_OBJECT__;
            if ( savedata->_data_type == KFDataDefine::DataTypeArray )
            {
                auto proto_array = &( ( *proto_object->mutable_pbarray() )[savedata->_data_setting->_name] );
                savedata = data_hierarchy.front();
                data_hierarchy.pop_front();

                ( *proto_array->mutable_pbuint64() )[ key ] = savedata->Get<int64>();
            }
            else
            {
                if ( savedata == data )
                {
                    AddSyncUpdateDataToPBObject( savedata, proto_object );
                }
            }
        } while ( !data_hierarchy.empty() );
    }

    void KFEntityEx::AddSyncUpdateDataToPBObject( DataPtr data, KFMsg::PBObject* proto_object )
    {
        auto data_setting = data->_data_setting;
        switch ( data->_data_type )
        {
        case KFDataDefine::DataTypeInt32:
            ( *proto_object->mutable_pbint32() )[data_setting->_name] = data->Get< int32 >();
            break;
        case KFDataDefine::DataTypeUInt32:
            ( *proto_object->mutable_pbuint32() )[data_setting->_name] = data->Get<uint32>();
            break;
        case KFDataDefine::DataTypeInt64:
            ( *proto_object->mutable_pbint64() )[data_setting->_name] = data->Get< int64 >();
            break;
        case KFDataDefine::DataTypeUInt64:
            ( *proto_object->mutable_pbuint64() )[data_setting->_name] = data->Get<uint64>();
            break;
        case KFDataDefine::DataTypeDouble:
            ( *proto_object->mutable_pbdouble() )[data_setting->_name] = data->Get<double>();
            break;
        case KFDataDefine::DataTypeString:
            ( *proto_object->mutable_pbstring() )[data_setting->_name] = data->Get< std::string >();
            break;
        case KFDataDefine::DataTypeObject:
            static_cast< KFKernelModule* >( _kf_kernel )->SaveToObject( data, proto_object, KFDataDefine::DataMaskSync );
            break;
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::SyncDataToClient()
    {
        // 同步更新
        SyncDataToClient( _default_sync_sequence );

        // 同步显示奖励
        SendShowElementToClient();
    }

    void KFEntityEx::SendShowElementToClient()
    {
        if ( !_have_show_client )
        {
            return;
        }

        _have_show_client = false;
        _component->_show_element_function.Call( __SHARED_FROM_ENTITY__, _show_elements );
        _show_elements.Clear();
    }

    KFMsg::PBObject* KFEntityEx::CreateSyncPBObject( uint32 type )
    {
        _have_sync_data = true;
        _component->AddSyncEntity( __SHARED_FROM_ENTITY__ );

        for ( auto i = 0u; i < __SYNC_COUNT__; ++i )
        {
            auto sync_data = &_sync_list[ i ];
            if ( sync_data->_type == type )
            {
                sync_data->_have_data = true;
                return &sync_data->_object;
            }
        }

        auto sync_data = &_sync_list[ 0u ];
        sync_data->_have_data = true;
        return &sync_data->_object;
    }

    void KFEntityEx::SyncDataToClient( uint32 first, uint32 second, uint32 third )
    {
        _have_sync_data = true;
        uint32 sync_sequence[ __SYNC_COUNT__ ] = { first, second, third };

        // 设置排序后, 把数据先同步客户端
        SyncDataToClient( sync_sequence );
    }

    void KFEntityEx::SyncDataToClient( const uint32* sync_sequence )
    {
        if ( !_have_sync_data )
        {
            return;
        }
        _have_sync_data = false;

        for ( auto i = 0u; i < __SYNC_COUNT__; ++i )
        {
            auto sync_data = &_sync_list[ i ];
            if ( sync_data->_have_data )
            {
                switch ( sync_data->_type )
                {
                case KFEnum::Add:
                    _component->_entity_sync_add_function.Call( __SHARED_FROM_ENTITY__, sync_data->_object );
                    break;
                case KFEnum::Dec:
                    _component->_entity_sync_remove_function.Call( __SHARED_FROM_ENTITY__, sync_data->_object );
                    break;
                case KFEnum::Set:
                    _component->_entity_sync_update_function.Call( __SHARED_FROM_ENTITY__, sync_data->_object );
                    break;
                }

                sync_data->_have_data = false;
                sync_data->_object.Clear();
            }

            sync_data->_type = sync_sequence[ i ];
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
#define __SYNC_UPDATE_INT_DATA__( data_name )\
    {\
        auto proto_data = &proto_object->data_name();\
        for ( auto iter = proto_data->begin(); iter != proto_data->end(); ++iter )\
        {\
            auto data = object_data->Find( iter->first );\
            if ( data != nullptr )\
            {\
                UpdateData( data, KFEnum::Set, iter->second );\
            }\
        }\
    }

#define __SYNC_UPDATE_STR_DATA__( data_name )\
    {\
        auto proto_data = &proto_object->data_name();\
        for ( auto iter = proto_data->begin(); iter != proto_data->end(); ++iter )\
        {\
            auto data = object_data->Find( iter->first );\
            if ( data != nullptr )\
            {\
                UpdateData( data, iter->second );\
            }\
        }\
    }

    void KFEntityEx::SyncUpdateDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object )
    {
        __SYNC_UPDATE_INT_DATA__( pbint32 );
        __SYNC_UPDATE_INT_DATA__( pbuint32 );
        __SYNC_UPDATE_INT_DATA__( pbint64 );
        __SYNC_UPDATE_INT_DATA__( pbuint64 );
        __SYNC_UPDATE_INT_DATA__( pbdouble );
        __SYNC_UPDATE_STR_DATA__( pbstring );

        // array
        {
            auto child_proto_array = &proto_object->pbarray();
            for ( auto iter = child_proto_array->begin(); iter != child_proto_array->end(); ++iter )
            {
                auto data = object_data->Find( iter->first );
                if ( data == nullptr )
                {
                    continue;
                }

                auto proto_uint64 = &iter->second.pbuint64();
                for ( auto value_iter = proto_uint64->begin(); value_iter != proto_uint64->end(); ++value_iter )
                {
                    auto child_data = data->Find( value_iter->first );
                    if ( child_data != nullptr )
                    {
                        UpdateData( child_data, value_iter->first, KFEnum::Set, value_iter->second );
                    }
                }
            }
        }


        // object
        {
            auto child_proto_object = &proto_object->pbobject();
            for ( auto iter = child_proto_object->begin(); iter != child_proto_object->end(); ++iter )
            {
                auto data = object_data->Find( iter->first );
                if ( data != nullptr )
                {
                    SyncUpdateDataFromServer( data, &iter->second );
                }
            }
        }

        // record
        {
            auto child_proto_record = &proto_object->pbrecord();
            for ( auto iter = child_proto_record->begin(); iter != child_proto_record->end(); ++iter )
            {
                auto record_data = object_data->Find( iter->first );
                if ( record_data == nullptr )
                {
                    continue;
                }

                auto child_proto_object = &iter->second.pbobject();
                for ( auto object_iter = child_proto_object->begin(); object_iter != child_proto_object->end(); ++object_iter )
                {
                    auto child_object = record_data->Find( object_iter->first );
                    if ( child_object != nullptr )
                    {
                        SyncUpdateDataFromServer( child_object, &object_iter->second );
                    }
                }
            }
        }
    }

    void KFEntityEx::SyncAddDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object )
    {
        // 对象
        {
            auto child_proto_object = &proto_object->pbobject();
            for ( auto iter = child_proto_object->begin(); iter != child_proto_object->end(); ++iter )
            {
                auto data = object_data->Find( iter->first );
                if ( data == nullptr )
                {
                    continue;
                }

                SyncAddDataFromServer( data, &iter->second );
            }
        }

        // 集合
        {
            auto child_proto_record = &proto_object->pbrecord();
            for ( auto iter = child_proto_record->begin(); iter != child_proto_record->end(); ++iter )
            {
                auto data = object_data->Find( iter->first );
                if ( data == nullptr )
                {
                    continue;
                }

                SyncAddRecordFormServer( data, &iter->second );
            }
        }
    }

    void KFEntityEx::SyncAddRecordFormServer( DataPtr record_data, const KFMsg::PBRecord* proto_record )
    {
        auto child_proto_object = &proto_record->pbobject();
        for ( auto iter = child_proto_object->begin(); iter != child_proto_object->end(); ++iter )
        {
            auto data = CreateData( record_data );
            auto ok = _kf_kernel->ParseFromMessage( data, &iter->second );
            if ( ok )
            {
                // 不存在列表时, 才更新
                if ( record_data->Find( data->GetKeyID() ) == nullptr )
                {
                    AddRecord( record_data, iter->first, data );
                }
            }
        }
    }

    void KFEntityEx::SyncRemoveDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object )
    {
        // object
        {
            auto child_proto_object = &proto_object->pbobject();
            for ( auto iter = child_proto_object->begin(); iter != child_proto_object->end(); ++iter )
            {
                auto data = object_data->Find( iter->first );
                if ( data != nullptr )
                {
                    SyncRemoveDataFromServer( data, &iter->second );
                }
            }
        }

        // record
        {
            auto child_proto_record = &proto_object->pbrecord();
            for ( auto iter = child_proto_record->begin(); iter != child_proto_record->end(); ++iter )
            {
                auto record_data = object_data->Find( iter->first );
                if ( record_data == nullptr )
                {
                    continue;
                }

                auto child_proto_object = &iter->second.pbobject();
                for ( auto object_iter = child_proto_object->begin(); object_iter != child_proto_object->end(); ++object_iter )
                {
                    RemoveRecord( record_data, object_iter->first );
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFEntityEx::GetConfigValue( const std::string& name, uint64 id, uint64 max_value )
    {
        auto function_data = _component->_get_config_value_function.Find( name );
        if ( function_data != nullptr )
        {
            return function_data->CallEx<uint64>( __SHARED_FROM_ENTITY__, id, max_value );
        }

        auto object_data = Find( name, id );
        if ( object_data != nullptr )
        {
            return object_data->Get<uint64>( object_data->_data_setting->_value_key_name );
        }

        return _invalid_int;
    }

    uint32 KFEntityEx::GetStatus()
    {
        return Get<uint32>( __STRING__( basic ), __STRING__( status ) );
    }

    void KFEntityEx::SetStatus( uint32 status )
    {
        auto basic_data = Find( __STRING__( basic ) );

        UpdateObject( basic_data, __STRING__( status ), KFEnum::Set, status );
        UpdateObject( basic_data, __STRING__( statustime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
    }
}