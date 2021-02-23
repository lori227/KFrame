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

    bool KFEntityEx::IsNew()
    {
        return _is_new;
    }

    void KFEntityEx::SetNew( bool isnew )
    {
        _is_new = isnew;
    }

    void KFEntityEx::InitEntity( KFComponentEx* kfcomponent )
    {
        _is_new = false;
        _is_inited = false;
        _is_in_save = false;
        _have_sync_data = false;
        _have_show_client = false;
        _kf_component = kfcomponent;
        for ( auto i = 0u; i < __SYNC_COUNT__; ++i )
        {
            _sync_list[ i ]._type = _default_sync_sequence[ i ];
        }

        _pb_show_elements.Clear();
    }

    DataPtr KFEntityEx::CreateData( const std::string& data_name )
    {
        auto kfdata = Find( data_name );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return CreateData( kfdata );
    }

    DataPtr KFEntityEx::CreateData( DataPtr kfdata )
    {
        auto child = KFDataFactory::Instance()->CreateData( kfdata->_data_setting, true );
        return child;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::UpdateData( DataPtr kfdata, const std::string& value, bool callback )
    {
        if ( kfdata->IsArray() )
        {
            kfdata->Set<std::string>( value );
            for ( auto i = KFGlobal::Instance()->_array_index; i < kfdata->Size(); ++i )
            {
                auto kfchild = kfdata->Find( i );
                if ( kfchild != nullptr )
                {
                    auto newvalue = kfchild->Get<uint64>();
                    _kf_component->UpdateDataCallBack( this, i, kfchild, i, KFEnum::Set, newvalue, 0u, newvalue, callback );
                }
            }
        }
        else if ( kfdata->IsString() )
        {
            // 属性更新回调
            auto oldvalue = kfdata->Get<std::string>();
            kfdata->Set<std::string>( value );
            _kf_component->UpdateDataCallBack( this, kfdata, oldvalue, value, callback );
        }
        else if ( kfdata->IsInt() )
        {
            auto intvalue = KFUtility::ToValue( value );
            UpdateData( kfdata, KFEnum::Set, intvalue, callback );
        }
    }

    void KFEntityEx::UpdateData( const std::string& data_name, const std::string& value, bool callback )
    {
        auto kfdata = Find( data_name );
        if ( kfdata == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, data_name );
        }

        UpdateData( kfdata, value, callback );
    }

    void KFEntityEx::UpdateObject( DataPtr kfparent, const std::string& data_name, const std::string& value, bool callback )
    {
        auto kfdata = kfparent->Find( data_name );
        if ( kfdata == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", kfparent->_data_setting->_name, data_name );
        }

        UpdateData( kfdata, value, callback );
    }

    void KFEntityEx::UpdateObject( const std::string& parent_name, const std::string& data_name, const std::string& value, bool callback )
    {
        auto kfparent = Find( parent_name );
        if ( kfparent == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parent_name );
        }

        UpdateObject( kfparent, data_name, value, callback );
    }

    void KFEntityEx::UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, const std::string& value, bool callback )
    {
        auto kfparent = Find( parent_name );
        if ( kfparent == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parent_name );
        }

        auto object_data = kfparent->Find( key );
        if ( object_data == nullptr )
        {
            return __LOG_ERROR__( "record=[{}] not have object=[{}]", parent_name, key );
        }

        UpdateObject( object_data, data_name, value, callback );
    }

    uint64 KFEntityEx::UpdateData( const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        auto kfdata = Find( data_name );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, data_name );
            return _invalid_int;
        }

        return UpdateData( _invalid_int, kfdata, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateData( DataPtr kfdata, uint32 operate, uint64 value, bool callback )
    {
        return UpdateData( _invalid_int, kfdata, operate, value );
    }

    uint64 KFEntityEx::UpdateData( uint64 key, DataPtr kfdata, uint32 operate, uint64 value, bool callback )
    {
        if ( !kfdata->IsInt() )
        {
            __LOG_ERROR__( "data=[{}] is not int=[{}]", kfdata->_data_setting->_name );
            return _invalid_int;
        }

        auto oldvalue = kfdata->Get<uint64>();
        auto newvalue = kfdata->Operate( operate, value );
        _kf_component->UpdateDataCallBack( this, key, kfdata, _invalid_int, operate, value, oldvalue, newvalue, callback );

        return newvalue;
    }

    uint64 KFEntityEx::UpdateObject( const std::string& parent_name, const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        auto kfparent = Find( parent_name );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parent_name );
            return _invalid_int;
        }

        return UpdateObject( kfparent, data_name, operate, value );
    }

    uint64 KFEntityEx::UpdateObject( DataPtr kfparent, const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        auto kfdata = kfparent->Find( data_name );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", kfparent->_data_setting->_name, data_name );
            return _invalid_int;
        }

        return UpdateData( kfparent->GetKeyID(), kfdata, operate, value );
    }

    uint64 KFEntityEx::UpdateRecord( const std::string& parent_name, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        auto kfparent = Find( parent_name );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, parent_name );
            return _invalid_int;
        }

        return UpdateRecord( kfparent, key, data_name, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateRecord( DataPtr kfparent, uint64 key, const std::string& data_name, uint32 operate, uint64 value, bool callback )
    {
        // 不存在, 创建
        auto object_data = kfparent->Find( key );
        if ( object_data == nullptr )
        {
            object_data = CreateData( kfparent );
            value = object_data->Operate( data_name, operate, value );
            AddRecord( kfparent, key, object_data, callback );
            return value;
        }

        // 存在就更新
        auto kfdata = object_data->Find( data_name );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "object=[{}] not have data=[{}]", object_data->_data_setting->_name, data_name );
            return _invalid_int;
        }

        return UpdateData( key, kfdata, operate, value, callback );
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

        return UpdateObjectArray( index, array_data, index, operate, value );
    }

    uint64 KFEntityEx::UpdateArray( DataPtr array_data, uint64 index, uint32 operate, uint64 value, bool callback )
    {
        return UpdateObjectArray( index, array_data, index, operate, value );
    }

    uint64 KFEntityEx::UpdateObjectArray( uint64 key, DataPtr array_data, uint64 index, uint32 operate, uint64 value, bool callback )
    {
        auto kfdata = array_data->Find( index );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "array=[{}] not index=[{}] max=[{}]", array_data->_data_setting->_name, index, array_data->MaxSize() );
            return _invalid_int;
        }

        auto oldvalue = kfdata->Get<uint64>();
        auto newvalue = kfdata->Operate( operate, value );
        _kf_component->UpdateDataCallBack( this, key, kfdata, index, operate, value, oldvalue, newvalue, callback );

        return newvalue;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::AddRecord( const std::string& parent_name, uint64 key, DataPtr kfdata, bool callback )
    {
        auto kfparent = Find( parent_name );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parent_name );
            return false;
        }

        if ( !kfparent->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", parent_name );
            return false;
        }

        if ( kfdata->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", kfdata->_data_setting->_name );
            return false;
        }

        return AddRecord( kfparent, key, kfdata, callback );
    }

    bool KFEntityEx::AddRecord( DataPtr kfparent, DataPtr kfdata, bool callback )
    {
        if ( !kfparent->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", kfparent->_data_setting->_name );
            return false;
        }

        if ( kfdata->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", kfdata->_data_setting->_name );
            return false;
        }

        auto key = kfdata->GetKeyID();
        return AddRecord( kfparent, key, kfdata, callback );
    }

    bool KFEntityEx::AddRecord( DataPtr kfparent, uint64 key, DataPtr kfdata, bool callback )
    {
        bool result = kfparent->Add( key, kfdata );
        if ( result )
        {
            // 添加属性回调
            _kf_component->AddDataCallBack( this, kfparent, key, kfdata, callback );
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
        auto kfdata = KFDataFactory::Instance()->AddArray( array_data );
        kfdata->Set( value );
        return kfdata;
    }

#define __ADD_ARRAY_DATA__( type )\
    void KFEntityEx::AddArray( DataPtr kfdata, const type& inlist )\
    {\
        if ( kfdata->IsArray() )\
        {\
            return __LOG_ERROR__( "data=[{}] is not array", kfdata->_data_setting->_name );\
        }\
        for ( auto value : inlist )\
        {\
            AddArray( kfdata, value );\
        }\
    }\

#define __ADD_ARRAY_DATA_1__( type )\
    void KFEntityEx::AddObjectArray( DataPtr kfdata, const std::string& data_name, const type& inlist )\
    {\
        if ( kfdata->IsObject() )\
        {\
            return __LOG_ERROR__( "data=[{}] is not object", kfdata->_data_setting->_name );\
        }\
        auto array_data = kfdata->Find( data_name );\
        if ( array_data == nullptr )\
        {\
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", kfdata->_data_setting->_name, data_name );\
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
        auto kfparent = Find( parent_name );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parent_name );
            return false;
        }

        return RemoveRecord( kfparent, key, callback );
    }

    bool KFEntityEx::RemoveRecord( DataPtr kfparent, uint64 key, bool callback )
    {
        if ( !kfparent->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", kfparent->_data_setting->_name );
            return false;
        }

        auto kfdata = kfparent->Find( key );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "record=[{}] not have object=[{}]", kfparent->_data_setting->_name, key );
            return false;
        }

        // 先移除属性
        kfparent->Move( key );
        _kf_component->RemoveDataCallBack( this, kfparent, key, kfdata, callback );
        KFDataFactory::Instance()->DestroyData( kfdata );
        return true;
    }

    bool KFEntityEx::ClearRecord( const std::string& parent_name, bool callback )
    {
        auto kfparent = Find( parent_name );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parent_name );
            return false;
        }

        return ClearRecord( kfparent, callback );
    }

    bool KFEntityEx::ClearRecord( DataPtr kfparent, bool callback )
    {
        if ( !kfparent->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", kfparent->_data_setting->_name );
            return false;
        }

        UInt64List keys;
        for ( auto kfdata = kfparent->First(); kfdata != nullptr; kfdata = kfparent->Next() )
        {
            keys.push_back( kfdata->GetKeyID() );
        }

        for ( auto key : keys )
        {
            RemoveRecord( kfparent, key, callback );
        }

        return true;
    }

    bool KFEntityEx::RemoveObject( DataPtr kfparent, const std::string& data_name, bool callback )
    {
        if ( !kfparent->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", kfparent->_data_setting->_name );
            return false;
        }

        auto kfdata = kfparent->Find( data_name );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", kfparent->_data_setting->_name, data_name );
            return false;
        }

        // 先移除属性
        kfparent->Move( data_name, false );
        _kf_component->RemoveDataCallBack( this, kfparent, 0, kfdata, callback );
        KFDataFactory::Instance()->DestroyData( kfdata );
        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFEntityEx::MoveRecord( DataPtr kfparent, uint64 key, bool callback )
    {
        if ( !kfparent->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", kfparent->_data_setting->_name );
            return nullptr;
        }

        // 移除属性
        auto kfdata = kfparent->Move( key );
        if ( kfdata != nullptr )
        {
            _kf_component->RemoveDataCallBack( this, kfparent, key, kfdata, callback );
        }
        else
        {
            __LOG_WARN__( "record=[{}] not have object=[{}]", kfparent->_data_setting->_name, key );
        }

        return kfdata;
    }


    DataPtr KFEntityEx::MoveRecordToRecord( const std::string& sourcename, uint64 key, const std::string& targetname )
    {
        auto sourcedata = Find( sourcename );
        auto targetdata = Find( targetname );
        if ( sourcedata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, sourcename );
            return nullptr;
        }

        if ( targetdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, targetname );
            return nullptr;
        }

        return MoveRecordToRecord( sourcedata, key, targetdata );
    }

    DataPtr KFEntityEx::MoveRecordToRecord( DataPtr sourcedata, uint64 key, DataPtr targetdata )
    {
        if ( !sourcedata->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", sourcedata->_data_setting->_name );
            return nullptr;
        }

        if ( !targetdata->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", targetdata->_data_setting->_name );
            return nullptr;
        }


        auto kfdata = sourcedata->Move( key );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "record=[{}] not have object=[{}]", sourcedata->_data_setting->_name, key );
            return nullptr;
        }

        _kf_component->RemoveDataCallBack( this, sourcedata, key, kfdata, false );

        targetdata->Add( key, kfdata );
        _kf_component->AddDataCallBack( this, targetdata, key, kfdata, false );

        return kfdata;
    }

    DataPtr KFEntityEx::MoveObjectToRecord( DataPtr sourcedata, const std::string& data_name, const std::string& targetname )
    {
        auto targetdata = Find( targetname );
        if ( targetdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, targetname );
            return nullptr;
        }

        return MoveObjectToRecord( sourcedata, data_name, targetdata );
    }

    DataPtr KFEntityEx::MoveObjectToRecord( DataPtr sourcedata, const std::string& data_name, DataPtr targetdata )
    {
        if ( !sourcedata->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", sourcedata->_data_setting->_name );
            return nullptr;
        }

        if ( !targetdata->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", targetdata->_data_setting->_name );
            return nullptr;
        }

        // 移除属性
        auto kfdata = sourcedata->Move( data_name, true );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "object=[{}] not have data=[{}]", sourcedata->_data_setting->_name, data_name );
            return nullptr;
        }
        _kf_component->RemoveDataCallBack( this, sourcedata, 0, kfdata, false );

        // 添加属性
        auto uuid = kfdata->GetKeyID();
        targetdata->Add( uuid, kfdata );
        _kf_component->AddDataCallBack( this, targetdata, uuid, kfdata, false );

        return kfdata;
    }

    DataPtr KFEntityEx::MoveRecordToObject( DataPtr sourcedata, uint64 key, DataPtr targetdata, const std::string& data_name )
    {
        if ( !sourcedata->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", sourcedata->_data_setting->_name );
            return nullptr;
        }

        if ( !targetdata->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", targetdata->_data_setting->_name );
            return nullptr;
        }

        // 移除属性
        auto kfdata = sourcedata->Move( key );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "record=[{}] not have object=[{}]", sourcedata->_data_setting->_name, key );
            return nullptr;
        }
        _kf_component->RemoveDataCallBack( this, sourcedata, key, kfdata, false );

        // 添加属性
        targetdata->Add( data_name, kfdata );
        SyncUpdateDataToClient( kfdata, key );

        return kfdata;
    }

    DataPtr KFEntityEx::MoveObjectToObject( DataPtr sourcedata, const std::string& sourcename, DataPtr targetdata, const std::string& targetname )
    {
        if ( !sourcedata->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", sourcedata->_data_setting->_name );
            return nullptr;
        }


        if ( !targetdata->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", targetdata->_data_setting->_name );
            return nullptr;
        }

        // 移除属性
        auto kfdata = sourcedata->Move( sourcename, true );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "object=[{}] not have object=[{}]", sourcedata->_data_setting->_name, sourcename );
            return nullptr;
        }
        _kf_component->RemoveDataCallBack( this, sourcedata, 0, kfdata, false );

        // 添加属性
        targetdata->Add( targetname, kfdata );
        SyncUpdateDataToClient( kfdata, 0u );

        return kfdata;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::LogElementResult( const KFElementResult* kfresult, const char* function, uint32 line )
    {
        // 打印日志
        switch ( kfresult->_show_type )
        {
        case KFDataShowEnum::Show_Element:
            LogElementResultElement( kfresult, function, line );
            break;
        case KFDataShowEnum::Show_Currency:
            LogElementResultCurrency( kfresult, function, line );
            break;
        case KFDataShowEnum::Show_Object:
            LogElementResultObject( kfresult, function, line );
            break;
        case KFDataShowEnum::Show_Overlay:
            LogElementResultOverlay( kfresult, function, line );
            break;
        case KFDataShowEnum::Show_NotOverlay:
            LogElementResultNotOverlay( kfresult, function, line );
            break;
        default:
            break;
        }
    }

    void KFEntityEx::ShowElementResult( const KFElementResult* kfresult )
    {
        auto ok = _kf_component->CallElementResultFunction( this, kfresult );
        if ( !ok )
        {
            return;
        }

        switch ( kfresult->_show_type )
        {
        case KFDataShowEnum::Show_Element:
            ShowElementResultElement( kfresult );
            break;
        case KFDataShowEnum::Show_Currency:
            ShowElementResultCurrency( kfresult );
            break;
        case KFDataShowEnum::Show_Object:
            ShowElementResultObject( kfresult );
            break;
        case KFDataShowEnum::Show_Overlay:
            ShowElementResultOverlay( kfresult );
            break;
        case KFDataShowEnum::Show_NotOverlay:
            ShowElementResultNotOverlay( kfresult );
            break;
        default:
            break;
        }
    }

    KFMsg::PBShowElement* KFEntityEx::CreateShowElement()
    {
        auto size = _pb_show_elements.element_size();
        if ( size <= 0 )
        {
            return nullptr;
        }

        return _pb_show_elements.mutable_element( size - 1 );
    }

    KFMsg::PBShowElement* KFEntityEx::CreateShowElement( const std::string& modulename, uint64 moduleid )
    {
        for ( auto i = 0; i < _pb_show_elements.element_size(); ++i )
        {
            auto pbshowelement = _pb_show_elements.mutable_element( i );
            if ( pbshowelement->modulename() == modulename &&
                    pbshowelement->moduleid() == moduleid )
            {
                return pbshowelement;
            }
        }

        _have_show_client = true;
        _kf_component->AddSyncEntity( this );

        auto pbshowelement = _pb_show_elements.add_element();
        pbshowelement->set_modulename( modulename );
        pbshowelement->set_moduleid( moduleid );
        return pbshowelement;
    }

    KFMsg::PBShowData* KFEntityEx::CreateShowData( KFMsg::PBShowElement* pbshowelement, const std::string& name, uint64 value, bool independ )
    {
        if ( !independ )
        {
            for ( auto i = 0; i < pbshowelement->pbdata_size(); ++i )
            {
                auto pbdata = pbshowelement->mutable_pbdata( i );
                if ( pbdata->name() != name )
                {
                    continue;
                }

                auto kfdatasetting = _kf_component->_data_setting->_class_setting->FindSetting( name );
                if ( kfdatasetting == nullptr )
                {
                    continue;
                }

                // 如果是record/object 判断value是否相同
                if ( kfdatasetting->_type == KFDataDefine::DataTypeRecord || kfdatasetting->_type == KFDataDefine::DataTypeObject )
                {
                    if ( pbdata->value() != value )
                    {
                        continue;
                    }
                }
                else
                {
                    pbdata->set_value( pbdata->value() + value );
                }

                return pbdata;
            }
        }

        auto pbdata = pbshowelement->add_pbdata();
        pbdata->set_name( name );
        pbdata->set_value( value );
        return pbdata;
    }

    bool KFEntityEx::IsElementResultShow( const KFElementResult* kfresult )
    {
        if ( !IsInited() || !kfresult->_is_need_show || kfresult->_module_name.empty() )
        {
            return false;
        }

        return true;
    }

    void KFEntityEx::LogElementResultElement( const KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( kfresult->_operate == KFEnum::Add )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] add=[{}]", _kf_component->_component_name, GetKeyID(), kfresult->_element->ToString() );
        }
        else
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] remove=[{}]", _kf_component->_component_name, GetKeyID(), kfresult->_element->ToString() );
        }
    }

    void KFEntityEx::ShowElementResultElement( const KFElementResult* kfresult )
    {
        if ( !IsElementResultShow( kfresult ) )
        {
            return;
        }

        auto pbshowelement = CreateShowElement( kfresult->_module_name, kfresult->_module_id );

        auto kfelement = kfresult->_element;
        if ( kfelement->IsValue() )
        {
            auto kfelementvalue = reinterpret_cast< const KFElementValue* >( kfelement );
            if ( kfelementvalue->_value->IsNeedShow() )
            {
                CreateShowData( pbshowelement, kfelementvalue->_data_name, kfelementvalue->_value->GetUseValue(), false );
            }
        }
        else if ( kfelement->IsObject() )
        {
            auto kfelementobject = reinterpret_cast< const KFElementObject* >( kfelement );
            if ( kfelementobject->IsNeedShow() )
            {
                auto pbshowdata = CreateShowData( pbshowelement, kfelementobject->_data_name, kfelementobject->_config_id, false );
                ( *pbshowdata->mutable_pbuint64() )[ __STRING__( id ) ] = kfelementobject->_config_id;
                for ( auto& iter : kfelementobject->_values._objects )
                {
                    auto kfvalue = iter.second;
                    if ( kfvalue->IsNeedShow() )
                    {
                        ( *pbshowdata->mutable_pbuint64() )[ iter.first ] = kfvalue->GetUseValue();
                    }
                }
            }
        }
    }

    void KFEntityEx::LogElementResultCurrency( const KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( kfresult->_operate == KFEnum::Add )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] add=[{{\"{}\":{}}}]",
                                   _kf_component->_component_name, GetKeyID(), kfresult->_element->_data_name, kfresult->_total_value );
        }
        else
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] remove=[{{\"{}\":{}}}]",
                                   _kf_component->_component_name, GetKeyID(), kfresult->_element->_data_name, kfresult->_total_value );
        }
    }


    void KFEntityEx::ShowElementResultCurrency( const KFElementResult* kfresult )
    {
        if ( IsElementResultShow( kfresult ) )
        {
            AddDataToShow( kfresult->_module_name, kfresult->_module_id, kfresult->_element->_data_name, kfresult->_total_value, true );
        }
    }

    void KFEntityEx::LogElementResultObject( const KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( kfresult->_operate == KFEnum::Add )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] add=[{{\"{}\":{{\"{}\":{}}}}}]",
                                   _kf_component->_component_name, GetKeyID(), kfresult->_element->_data_name, kfresult->_data_name, kfresult->_total_value );
        }
        else
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] remove=[{{\"{}\":{{\"{}\":{}}}}}]",
                                   _kf_component->_component_name, GetKeyID(), kfresult->_element->_data_name, kfresult->_data_name, kfresult->_total_value );
        }
    }

    void KFEntityEx::ShowElementResultObject( const KFElementResult* kfresult )
    {
        if ( IsElementResultShow( kfresult ) )
        {
            AddDataToShow( kfresult->_module_name, kfresult->_module_id, kfresult->_element->_data_name, kfresult->_total_value, true );
        }
    }

    void KFEntityEx::LogElementResultOverlay( const KFElementResult* kfresult, const char* function, uint32 line )
    {
        for ( auto& iter : kfresult->_overlay_list )
        {
            if ( kfresult->_operate == KFEnum::Add )
            {
                __LOG_INFO_FUNCTION__( function, line, "{}=[{}] extend=[{}] add=[{{\"{}\":{{\"{}\":{},\"{}\":{}}}}}]",
                                       _kf_component->_component_name, GetKeyID(), iter.first,
                                       kfresult->_element->_data_name, __STRING__( id ), kfresult->_config_id, kfresult->_data_name, iter.second );
            }
            else
            {
                __LOG_INFO_FUNCTION__( function, line, "{}=[{}] extend=[{}] remove=[{{\"{}\":{{\"{}\":{},\"{}\":{}}}}}]",
                                       _kf_component->_component_name, GetKeyID(), iter.first,
                                       kfresult->_element->_data_name, __STRING__( id ), kfresult->_config_id, kfresult->_data_name, iter.second );
            }
        }
    }

    void KFEntityEx::ShowElementResultOverlay( const KFElementResult* kfresult )
    {
        for ( auto& iter : kfresult->_overlay_list )
        {
            if ( IsElementResultShow( kfresult ) )
            {
                StringUInt64 values;
                values[ kfresult->_data_name ] = iter.second;
                values[ __STRING__( id ) ] = kfresult->_config_id;
                AddDataToShow( kfresult->_module_name, kfresult->_module_id, kfresult->_element->_data_name, kfresult->_config_id, values, true );
            }
        }
    }

    void KFEntityEx::LogElementResultNotOverlay( const KFElementResult* kfresult, const char* function, uint32 line )
    {
        for ( auto kfdata : kfresult->_not_overlay_list )
        {
            if ( kfresult->_operate == KFEnum::Add )
            {
                __LOG_INFO_FUNCTION__( function, line, "{}=[{}] extend=[{}] add=[{{\"{}\":{}}}]",
                                       _kf_component->_component_name, GetKeyID(), kfdata->_data_setting->_name,
                                       kfresult->_element->_data_name, kfdata->ToString() );
            }
            else
            {
                __LOG_INFO_FUNCTION__( function, line, "{}=[{}] extend=[{}] remove=[{{\"{}\":{}}}]",
                                       _kf_component->_component_name, GetKeyID(), kfdata->_data_setting->_name,
                                       kfresult->_element->_data_name, kfdata->ToString() );
            }
        }
    }


    void KFEntityEx::ShowElementResultNotOverlay( const KFElementResult* kfresult )
    {
        for ( auto kfdata : kfresult->_not_overlay_list )
        {
            if ( IsElementResultShow( kfresult ) )
            {
                AddDataToShow( kfresult->_module_name, kfresult->_module_id, kfdata );
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::SetDataShowModule( const std::string& modulename, uint64 moduleid )
    {
        if ( modulename.empty() )
        {
            return;
        }

        CreateShowElement( modulename, moduleid );
    }

    void KFEntityEx::AddDataToShow( const std::string& modulename, uint64 moduleid, DataPtr kfdata, bool independ )
    {
        auto pbshowelement = CreateShowElement( modulename, moduleid );
        AddDataToShowElement( pbshowelement, kfdata, independ );
    }

    void KFEntityEx::AddDataToShow( const std::string& name, uint64 value, bool independ )
    {
        auto pbshowelement = CreateShowElement();
        if ( pbshowelement == nullptr )
        {
            return;
        }

        CreateShowData( pbshowelement, name, value, independ );
    }

    void KFEntityEx::AddDataToShow( const std::string& modulename, uint64 moduleid, const std::string& name, uint64 value, bool independ )
    {
        if ( value == 0u )
        {
            return;
        }

        auto pbshowelement = CreateShowElement( modulename, moduleid );
        CreateShowData( pbshowelement, name, value, independ );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddDataToShow( const std::string& name, uint64 value, StringUInt64& values, bool independ )
    {
        auto pbshowelement = CreateShowElement();
        if ( pbshowelement == nullptr )
        {
            return;
        }

        AddDataToShowElement( pbshowelement, name, value, values, independ );
    }

    void KFEntityEx::AddDataToShow( const std::string& modulename, uint64 moduleid, const std::string& name, uint64 value, StringUInt64& values, bool independ )
    {
        auto pbshowelement = CreateShowElement( modulename, moduleid );
        AddDataToShowElement( pbshowelement, name, value, values, independ );
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddDataToShowElement( KFMsg::PBShowElement* pbshowelement, DataPtr kfdata, bool independ )
    {
        if ( !kfdata->_data_setting->HaveMask( KFDataDefine::DataMaskShow ) )
        {
            return;
        }

        switch ( kfdata->_data_type )
        {
        case KFDataDefine::DataTypeInt32:
        case KFDataDefine::DataTypeUInt32:
        case KFDataDefine::DataTypeInt64:
        case KFDataDefine::DataTypeUInt64:
        {
            CreateShowData( pbshowelement, kfdata->_data_setting->_logic_name, kfdata->Get(), independ );
            break;
        }
        case KFDataDefine::DataTypeObject:
        case KFDataDefine::DataTypeRecord:
        {
            auto configid = kfdata->Get( kfdata->_data_setting->_config_key_name );
            auto pbshowdata = CreateShowData( pbshowelement, kfdata->_data_setting->_logic_name, configid, false );
            for ( auto kfchild = kfdata->First(); kfchild != nullptr; kfchild = kfdata->Next() )
            {
                if ( !kfchild->_data_setting->HaveMask( KFDataDefine::DataMaskShow ) ||
                        !kfchild->IsValid() )
                {
                    continue;
                }

                ( *pbshowdata->mutable_pbuint64() )[ kfchild->_data_setting->_logic_name] = kfchild->Get();
            }
            break;
        }
        }
    }

    void KFEntityEx::AddDataToShowElement( KFMsg::PBShowElement* pbshowelement, const std::string& name, uint64 value, StringUInt64& values, bool independ )
    {
        auto pbshowdata = CreateShowData( pbshowelement, name, value, independ );
        for ( auto& iter : values )
        {
            if ( iter.first == __STRING__( count ) )
            {
                ( *pbshowdata->mutable_pbuint64() )[ iter.first ] += iter.second;
            }
            else
            {
                ( *pbshowdata->mutable_pbuint64() )[ iter.first ] = iter.second;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFEntityEx::FindByLogicName( const std::string& name )
    {
        auto kfdatasetting = _data_setting->_class_setting->FindSettingByLogicName( name );
        if ( kfdatasetting == nullptr )
        {
            return nullptr;
        }

        return Find( kfdatasetting->_name );
    }

    const std::string& KFEntityEx::CheckAddElement( const KFElements* elements, double multiple, const char* function, uint32 line )
    {
        for ( auto kfelement : elements->_element_list )
        {
            auto ok = CheckAddElement( kfelement, multiple, function, line );
            if ( !ok )
            {
                return kfelement->_data_name;
            }
        }

        return _invalid_string;
    }

    bool KFEntityEx::CheckAddElement( const KFElement* kfelement, double multiple, const char* function, uint32 line )
    {
        auto kfdata = Find( kfelement->_data_name );
        if ( kfdata == nullptr )
        {
            kfdata = FindByLogicName( kfelement->_data_name );
            if ( kfdata == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "can't find data=[{}]", kfelement->_data_name );
                return false;
            }
        }

        // 如果有注册函数, 执行注册函数
        auto kffunction = _kf_component->_check_add_element_function.Find( kfdata->_data_setting->_logic_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( this, kfdata, const_cast< KFElement* >( kfelement ), multiple, function, line );
        }

        // 找不到处理函数, 用基础函数来处理
        return !kfdata->IsFull();
    }

    void KFEntityEx::AddElement( const KFElements* elements, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        if ( elements->_element_list.empty() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "{}=[{}] element=[{}] is empty", _kf_component->_component_name, GetKeyID(), elements->GetElement() );
        }

        __LOG_INFO_FUNCTION__( function, line, "{}=[{}] start add elements={} multiple=[{:0.2f}]", _kf_component->_component_name, GetKeyID(), elements->GetElement(), multiple );
        _element_sequence = KFGlobal::Instance()->STMakeUuid();
        for ( auto kfelement : elements->_element_list )
        {
            AddElement( kfelement, multiple, modulename, moduleid, function, line );
        }
        __LOG_INFO_FUNCTION__( function, line, "{}=[{}] finish add elements", _kf_component->_component_name, GetKeyID() );
    }

    // 添加元数据
    void KFEntityEx::AddElement( const KFElement* kfelement, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        auto kfdata = Find( kfelement->_data_name );
        if ( kfdata == nullptr )
        {
            kfdata = FindByLogicName( kfelement->_data_name );
            if ( kfdata == nullptr )
            {
                return __LOG_ERROR_FUNCTION__( function, line, "can't find data=[{}]", kfelement->_data_name );
            }
        }

        const_cast< KFElement* >( kfelement )->_data_setting = kfdata->_data_setting;

        KFElementResult elementresult;
        elementresult._multiple = multiple;
        elementresult._operate = KFEnum::Add;
        elementresult._sequence = _element_sequence;
        elementresult._element = const_cast< KFElement* >( kfelement );
        elementresult._is_need_show = kfdata->_data_setting->HaveMask( KFDataDefine::DataMaskShow );
        elementresult._module_name = modulename;
        elementresult._module_id = moduleid;

        auto ok = false;
        auto kffunction = _kf_component->_add_element_function.Find( kfdata->_data_setting->_logic_name );
        if ( kffunction != nullptr )
        {
            ok = kffunction->_function( this, kfdata, &elementresult, function, line );
        }
        else
        {
            // 没有注册的函数
            switch ( kfdata->_data_type )
            {
            case KFDataDefine::DataTypeObject:
                ok = AddObjectElement( kfdata, &elementresult, function, line );
                break;
            case KFDataDefine::DataTypeRecord:
                ok = AddRecordElement( kfdata, &elementresult, function, line );
                break;
            default:
                ok = AddNormalElement( kfdata, &elementresult, function, line );
                break;
            }
        }

        if ( ok )
        {
            // 显示属性
            ShowElementResult( &elementresult );

            // 打印日志
            LogElementResult( &elementresult, function, line );

            // 运行日志
            _kf_component->CallLogElementFunction( this, &elementresult );
        }
    }

    bool KFEntityEx::AddNormalElement( DataPtr kfdata, KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( !kfresult->_element->IsValue() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value", kfresult->_element->_data_name );
            return false;
        }

        auto kfelementvalue = reinterpret_cast< KFElementValue* >( kfresult->_element );
        switch ( kfelementvalue->_value->_type )
        {
        case KFDataDefine::DataTypeUInt32:
        {
            auto usevalue = kfelementvalue->_value->CalcUseValue( kfdata->_data_setting, kfresult->_multiple );
            UpdateData( kfdata, kfelementvalue->_operate, usevalue );

            kfresult->AddResult( usevalue );
        }
        break;
        case KFDataDefine::DataTypeString:
            UpdateData( kfdata, kfelementvalue->_value->GetValue() );
            break;
        default:
            break;
        }

        return true;
    }

    void KFEntityEx::UpdateElementToData( DataPtr kfdata, KFElementObject* kfelement, double multiple )
    {
        for ( auto& iter : kfelement->_values._objects )
        {
            auto kfchild = kfdata->Find( iter.first );
            if ( kfchild == nullptr ||
                    kfchild->_data_setting->_name == kfdata->_data_setting->_key_name ||
                    kfchild->_data_setting->_name == kfdata->_data_setting->_config_key_name )
            {
                continue;
            }

            auto kfvalue = iter.second;
            switch ( kfvalue->_type )
            {
            case KFDataDefine::DataTypeUInt32:
                UpdateData( kfchild, kfelement->_operate, kfvalue->CalcUseValue( kfchild->_data_setting, multiple ) );
                break;
            case KFDataDefine::DataTypeString:
                UpdateData( kfchild, kfvalue->GetValue() );
                break;
            case KFDataDefine::DataTypeObject:
            {
                auto kfobjvalue = static_cast< KFObjValue* >( kfvalue );
                kfobjvalue->_element->SetOperate( kfelement->_operate );
                UpdateElementToData( kfchild, kfobjvalue->_element, multiple );
            }
            break;
            default:
                break;
            }
        }
    }

    void KFEntityEx::SetElementToData( DataPtr kfdata, KFElementObject* kfelement, double multiple )
    {
        for ( auto& iter : kfelement->_values._objects )
        {
            auto kfchild = kfdata->Find( iter.first );
            if ( kfchild == nullptr )
            {
                continue;
            }

            auto kfvalue = iter.second;
            switch ( kfvalue->_type )
            {
            case KFDataDefine::DataTypeUInt32:
                kfchild->Set( kfvalue->CalcUseValue( kfchild->_data_setting, multiple ) );
                break;
            case KFDataDefine::DataTypeString:
                kfchild->Set( kfvalue->GetValue() );
                break;
            case KFDataDefine::DataTypeObject:
                SetElementToData( kfchild, static_cast< KFObjValue* >( kfvalue )->_element, multiple );
                break;
            default:
                break;
            }
        }
    }

    bool KFEntityEx::AddObjectElement( DataPtr kfparent, KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( !kfresult->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfresult->_element->_data_name );
            return false;
        }

        UpdateElementToData( kfparent, reinterpret_cast< KFElementObject* >( kfresult->_element ), kfresult->_multiple );
        return true;
    }

    bool KFEntityEx::AddRecordElement( DataPtr kfparent, KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( !kfresult->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfresult->_element->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfresult->_element );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfresult->_element->_data_name );
            return false;
        }

        auto kfchild = kfparent->Find( kfelementobject->_config_id );
        if ( kfchild == nullptr )
        {
            // 不存在, 创建一个, 并更新属性
            kfchild = CreateData( kfparent );
            SetElementToData( kfchild, kfelementobject, kfresult->_multiple );
            AddRecord( kfparent, kfelementobject->_config_id, kfchild );
        }
        else
        {
            // 存在直接更新属性
            UpdateElementToData( kfchild, kfelementobject, kfresult->_multiple );
        }

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 判断元数据是否满足条件
    const std::string& KFEntityEx::CheckRemoveElement( const KFElements* elements, double multiple, const char* function, uint32 line )
    {
        for ( auto kfelement : elements->_element_list )
        {
            auto ok = CheckRemoveElement( kfelement, multiple, function, line  );
            if ( !ok )
            {
                return kfelement->_data_name;
            }
        }

        return _invalid_string;
    }

    bool KFEntityEx::CheckRemoveElement( const KFElement* kfelement, double multiple, const char* function, uint32 line )
    {
        auto kfdata = Find( kfelement->_data_name );
        if ( kfdata == nullptr )
        {
            kfdata = FindByLogicName( kfelement->_data_name );
            if ( kfdata == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "can't find data=[{}]", kfelement->_data_name );
                return false;
            }
        }

        // 如果有注册函数, 执行注册函数
        auto kffunction = _kf_component->_check_remove_element_function.Find( kfdata->_data_setting->_logic_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( this, kfdata, const_cast< KFElement* >( kfelement ), multiple, function, line );
        }

        // 找不到处理函数, 用基础函数来处理
        switch ( kfdata->_data_type )
        {
        case KFDataDefine::DataTypeRecord:
            return CheckRecordElement( kfdata, const_cast< KFElement* >( kfelement ), multiple, function, line );
            break;
        case KFDataDefine::DataTypeObject:
            return CheckObjectElement( kfdata, const_cast< KFElement* >( kfelement ), multiple, function, line );
            break;
        default:
            return CheckNormalElement( kfdata, const_cast< KFElement* >( kfelement ), multiple, function, line );
            break;
        }

        return false;
    }

    bool KFEntityEx::CheckNormalElement( DataPtr kfdata, KFElement* kfelement, double multiple, const char* function, uint32 line )
    {
        if ( !kfelement->IsValue() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value", kfelement->_data_name );
            return false;
        }

        auto kfelementvalue = reinterpret_cast< KFElementValue* >( kfelement );
        if ( !kfelementvalue->_value->IsType( KFDataDefine::DataTypeUInt32 ) )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", kfelement->_data_name );
            return false;
        }

        auto data_value = kfdata->Get();
        auto elementvalue = kfelementvalue->_value->CalcUseValue( kfdata->_data_setting, multiple );
        return data_value >= elementvalue;
    }

    bool KFEntityEx::CheckObjectElement( DataPtr kfparent, KFElement* kfelement, double multiple, const char* function, uint32 line )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto& name = iter.first;
            auto kfvalue = iter.second;
            if ( !kfvalue->IsType( KFDataDefine::DataTypeUInt32 ) )
            {
                __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", name );
                return false;
            }

            auto kfchild = kfparent->Find( name );
            if ( kfchild == nullptr )
            {
                return false;
            }

            auto data_value = kfchild->Get() ;
            auto elementvalue = kfvalue->CalcUseValue( kfchild->_data_setting, multiple );
            if ( data_value < elementvalue )
            {
                return false;
            }
        }

        return true;
    }

    bool KFEntityEx::CheckRecordElement( DataPtr kfparent, KFElement* kfelement, double multiple, const char* function, uint32 line )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfelement->_data_name );
            return false;
        }

        auto kfdata = kfparent->Find( kfelementobject->_config_id );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return CheckObjectElement( kfdata, kfelement, multiple, function, line );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    const std::string& KFEntityEx::RemoveElement( const KFElements* elements, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        if ( elements->IsEmpty() )
        {
            return _invalid_string;
        }

        auto& data_name = CheckRemoveElement( elements, multiple, function, line );
        if ( data_name.empty() )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] start remove elements={} multiple=[{:0.2f}]", _kf_component->_component_name, GetKeyID(), elements->GetElement(), multiple );
            _element_sequence = KFGlobal::Instance()->STMakeUuid();
            for ( auto kfelement : elements->_element_list )
            {
                RemoveElement( kfelement, multiple, modulename, moduleid, function, line );
            }
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] finish remove elements", _kf_component->_component_name, GetKeyID() );
        }

        return data_name;
    }

    // 删除元数据
    void KFEntityEx::RemoveElement( const KFElement* kfelement, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        auto kfdata = Find( kfelement->_data_name );
        if ( kfdata == nullptr )
        {
            kfdata = FindByLogicName( kfelement->_data_name );
            if ( kfdata == nullptr )
            {
                return __LOG_ERROR_FUNCTION__( function, line, "can't find data=[{}]", kfelement->_data_name );
            }
        }

        KFElementResult elementresult;
        elementresult._multiple = multiple;
        elementresult._operate = KFEnum::Dec;
        elementresult._sequence = _element_sequence;
        elementresult._element = const_cast< KFElement* >( kfelement );
        elementresult._module_name = modulename;
        elementresult._module_id = moduleid;

        auto ok = false;
        auto kffunction = _kf_component->_remove_element_function.Find( kfdata->_data_setting->_logic_name );
        if ( kffunction != nullptr )
        {
            ok = kffunction->_function( this, kfdata, &elementresult, function, line );
        }
        else
        {
            // 找不到处理函数, 用基础函数来处理
            switch ( kfdata->_data_type )
            {
            case KFDataDefine::DataTypeRecord:
                ok = RemoveRecordElement( kfdata, &elementresult, function, line );
                break;
            case KFDataDefine::DataTypeObject:
                ok = RemoveObjectElement( kfdata, &elementresult, function, line );
                break;
            default:
                ok = RemoveNormalElement( kfdata, &elementresult, function, line );
                break;
            }
        }

        if ( ok )
        {
            // 打印日志
            LogElementResult( &elementresult, function, line );

            // 运营日志
            _kf_component->CallLogElementFunction( this, &elementresult );
        }
    }

    bool KFEntityEx::RemoveNormalElement( DataPtr kfdata, KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( !kfresult->_element->IsValue() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value", kfresult->_element->_data_name );
            return false;
        }

        auto kfelementvalue = reinterpret_cast< KFElementValue* >( kfresult->_element );
        if ( !kfelementvalue->_value->IsType( KFDataDefine::DataTypeUInt32 ) )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", kfresult->_element->_data_name );
            return false;
        }

        auto usevalue = kfelementvalue->_value->GetUseValue();
        UpdateData( kfdata, KFEnum::Dec, usevalue );

        // 添加结果
        kfresult->AddResult( usevalue );
        return true;
    }

    bool KFEntityEx::RemoveObjectElement( DataPtr object_data, KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( !kfresult->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfresult->_element->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfresult->_element );
        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto& name = iter.first;
            auto kfvalue = iter.second;
            if ( !kfvalue->IsType( KFDataDefine::DataTypeUInt32 ) )
            {
                __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", name );
                continue;
            }

            UpdateObject( object_data, name, KFEnum::Dec, kfvalue->GetUseValue() );
        }

        return true;
    }

    bool KFEntityEx::RemoveRecordElement( DataPtr kfparent, KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( !kfresult->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfresult->_element->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfresult->_element );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfresult->_element->_data_name );
            return false;
        }

        auto object_data = kfparent->Find( kfelementobject->_config_id );
        if ( object_data == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] can't find id=[{}]", kfelementobject->_data_name, kfelementobject->_config_id );
            return false;
        }

        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto& name = iter.first;
            auto kfvalue = iter.second;
            if ( !kfvalue->IsType( KFDataDefine::DataTypeUInt32 ) )
            {
                __LOG_ERROR_FUNCTION__( function, line, "element=[{}] is not int", name );
                continue;
            }

            UpdateObject( object_data, name, KFEnum::Dec, kfvalue->GetUseValue() );
        }

        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
#define __PREPARE_SYNC__( function )\
    if ( !function.IsOpen() ||\
            !IsInited() || !kfdata->IsNeedSyncToClient() )\
    {\
        return;\
    }\
    std::list< DataPtr > datahierarchy;\
    datahierarchy.push_front( kfdata );\
    auto kfparent = kfdata->GetParent();\
    while ( kfparent != nullptr )\
    {\
        if ( !kfparent->IsNeedSyncToClient() )\
        {\
            return;\
        }\
        datahierarchy.push_front( kfparent );\
        kfparent = kfparent->GetParent();\
    }\
    datahierarchy.pop_front();\
    if ( datahierarchy.empty() )\
    {\
        return;\
    }\

#define __FIND_PROTO_OBJECT__\
    auto savedata = datahierarchy.front();\
    datahierarchy.pop_front();\
    if ( savedata->_data_type == KFDataDefine::DataTypeObject )\
    {\
        proto_object = &( ( *proto_object->mutable_pbobject() )[ savedata->_data_setting->_name ] );\
    }\
    else if ( savedata->_data_type == KFDataDefine::DataTypeRecord )\
    {\
        auto pbrecord = &( ( *proto_object->mutable_pbrecord() )[ savedata->_data_setting->_name ] );\
        savedata = datahierarchy.front();\
        datahierarchy.pop_front(); \
        proto_object = &( ( *pbrecord->mutable_pbobject() )[ savedata->GetKeyID() ] ); \
    }\

    void KFEntityEx::SyncAddDataToClient( DataPtr kfdata, uint64 key )
    {
        __PREPARE_SYNC__( _kf_component->_entity_sync_add_function );
        auto proto_object = CreateSyncPBObject( KFEnum::Add );
        do
        {
            __FIND_PROTO_OBJECT__;
            if ( savedata == kfdata )
            {
                static_cast<KFKernelModule*>( _kf_kernel )->SaveToObject( savedata, proto_object, KFDataDefine::DataMaskSync );
            }
        } while ( !datahierarchy.empty() );
    }

    void KFEntityEx::SyncRemoveDataToClient( DataPtr kfdata, uint64 key )
    {
        __PREPARE_SYNC__( _kf_component->_entity_sync_remove_function );
        auto proto_object = CreateSyncPBObject( KFEnum::Dec );
        do
        {
            __FIND_PROTO_OBJECT__;
        } while ( !datahierarchy.empty() );
    }

    void KFEntityEx::SynAddRecordDataToClient( DataPtr kfdata )
    {
        for ( auto kfchild = kfdata->First(); kfchild != nullptr; kfchild = kfdata->Next() )
        {
            SyncAddDataToClient( kfchild, kfchild->GetKeyID() );
        }
    }

    void KFEntityEx::SyncUpdateDataToClient( DataPtr kfdata, uint64 key )
    {
        __PREPARE_SYNC__( _kf_component->_entity_sync_update_function );
        auto proto_object = CreateSyncPBObject( KFEnum::Set );
        do
        {
            __FIND_PROTO_OBJECT__;
            if ( savedata->_data_type == KFDataDefine::DataTypeArray )
            {
                auto pbarray = &( ( *proto_object->mutable_pbarray() )[ savedata->_data_setting->_name ] );
                savedata = datahierarchy.front();
                datahierarchy.pop_front();

                ( *pbarray->mutable_pbuint64() )[ key ] = savedata->Get<int64>();
            }
            else
            {
                if ( savedata == kfdata )
                {
                    AddSyncUpdateDataToPBObject( savedata, proto_object );
                }
            }
        } while ( !datahierarchy.empty() );
    }

    void KFEntityEx::AddSyncUpdateDataToPBObject( DataPtr kfdata, KFMsg::PBObject* proto_object )
    {
        auto datasetting = kfdata->_data_setting;
        switch ( kfdata->_data_type )
        {
        case KFDataDefine::DataTypeInt32:
            ( *proto_object->mutable_pbint32() )[ datasetting->_name ] = kfdata->Get< int32 >();
            break;
        case KFDataDefine::DataTypeUInt32:
            ( *proto_object->mutable_pbuint32() )[ datasetting->_name ] = kfdata->Get<uint32>();
            break;
        case KFDataDefine::DataTypeInt64:
            ( *proto_object->mutable_pbint64() )[ datasetting->_name ] = kfdata->Get< int64 >();
            break;
        case KFDataDefine::DataTypeUInt64:
            ( *proto_object->mutable_pbuint64() )[ datasetting->_name ] = kfdata->Get<uint64>();
            break;
        case KFDataDefine::DataTypeDouble:
            ( *proto_object->mutable_pbdouble() )[ datasetting->_name ] = kfdata->Get<double>();
            break;
        case KFDataDefine::DataTypeString:
            ( *proto_object->mutable_pbstring() )[ datasetting->_name ] = kfdata->Get< std::string >();
            break;
        case KFDataDefine::DataTypeObject:
            static_cast< KFKernelModule* >( _kf_kernel )->SaveToObject( kfdata, proto_object, KFDataDefine::DataMaskSync );
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
        _kf_component->_show_element_function.Call( this, _pb_show_elements );
        _pb_show_elements.Clear();
    }

    KFMsg::PBObject* KFEntityEx::CreateSyncPBObject( uint32 type )
    {
        _have_sync_data = true;
        _kf_component->AddSyncEntity( this );

        for ( auto i = 0u; i < __SYNC_COUNT__; ++i )
        {
            auto kfsyncdata = &_sync_list[ i ];
            if ( kfsyncdata->_type == type )
            {
                kfsyncdata->_have_data = true;
                return &kfsyncdata->_pbobject;
            }
        }

        auto kfsyncdata = &_sync_list[ 0u ];
        kfsyncdata->_have_data = true;
        return &kfsyncdata->_pbobject;
    }

    void KFEntityEx::SyncDataToClient( uint32 first, uint32 second, uint32 third )
    {
        _have_sync_data = true;
        uint32 syncsequence[ __SYNC_COUNT__ ] = { first, second, third };

        // 设置排序后, 把数据先同步客户端
        SyncDataToClient( syncsequence );
    }

    void KFEntityEx::SyncDataToClient( const uint32* syncsequence )
    {
        if ( !_have_sync_data )
        {
            return;
        }
        _have_sync_data = false;

        for ( auto i = 0u; i < __SYNC_COUNT__; ++i )
        {
            auto kfsyncdata = &_sync_list[ i ];
            if ( kfsyncdata->_have_data )
            {
                switch ( kfsyncdata->_type )
                {
                case KFEnum::Add:
                    _kf_component->_entity_sync_add_function.Call( this, kfsyncdata->_pbobject );
                    break;
                case KFEnum::Dec:
                    _kf_component->_entity_sync_remove_function.Call( this, kfsyncdata->_pbobject );
                    break;
                case KFEnum::Set:
                    _kf_component->_entity_sync_update_function.Call( this, kfsyncdata->_pbobject );
                    break;
                }

                kfsyncdata->_have_data = false;
                kfsyncdata->_pbobject.Clear();
            }

            kfsyncdata->_type = syncsequence[ i ];
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
#define __SYNC_UPDATE_INT_DATA__( datatype, pbdata )\
    {\
        auto pbdata = &proto_object->pbdata();\
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )\
        {\
            auto kfdata = object_data->Find( iter->first );\
            if ( kfdata != nullptr )\
            {\
                UpdateData( kfdata, KFEnum::Set, iter->second );\
            }\
        }\
    }

#define __SYNC_UPDATE_STR_DATA__( datatype, pbdata )\
    {\
        auto pbdata = &proto_object->pbdata();\
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )\
        {\
            auto kfdata = object_data->Find( iter->first );\
            if ( kfdata != nullptr )\
            {\
                UpdateData( kfdata, iter->second );\
            }\
        }\
    }

    void KFEntityEx::SyncUpdateDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object )
    {
        __SYNC_UPDATE_INT_DATA__( int32, pbint32 );
        __SYNC_UPDATE_INT_DATA__( uint32, pbuint32 );
        __SYNC_UPDATE_INT_DATA__( int64, pbint64 );
        __SYNC_UPDATE_INT_DATA__( uint64, proto_uint64 );
        __SYNC_UPDATE_INT_DATA__( double, pbdouble );
        __SYNC_UPDATE_STR_DATA__( std::string, pbstring );

        // array
        {
            auto pbarray = &proto_object->pbarray();
            for ( auto iter = pbarray->begin(); iter != pbarray->end(); ++iter )
            {
                auto kfdata = object_data->Find( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                auto proto_uint64 = &iter->second.proto_uint64();
                for ( auto citer = proto_uint64->begin(); citer != proto_uint64->end(); ++citer )
                {
                    auto kfchilddata = kfdata->Find( citer->first );
                    if ( kfchilddata != nullptr )
                    {
                        UpdateData( kfchilddata, citer->first, KFEnum::Set, citer->second );
                    }
                }
            }
        }


        // object
        {
            auto pbchild = &proto_object->proto_object();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = object_data->Find( iter->first );
                if ( kfdata != nullptr )
                {
                    SyncUpdateDataFromServer( kfdata, &iter->second );
                }
            }
        }

        // record
        {
            auto pbchild = &proto_object->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfrecord = object_data->Find( iter->first );
                if ( kfrecord == nullptr )
                {
                    continue;
                }

                auto pbchildobject = &iter->second.proto_object();
                for ( auto citer = pbchildobject->begin(); citer != pbchildobject->end(); ++citer )
                {
                    auto kfchildobject = kfrecord->Find( citer->first );
                    if ( kfchildobject != nullptr )
                    {
                        SyncUpdateDataFromServer( kfchildobject, &citer->second );
                    }
                }
            }
        }
    }

    void KFEntityEx::SyncAddDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object )
    {
        // 对象
        {
            auto pbchild = &proto_object->proto_object();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = object_data->Find( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                SyncAddDataFromServer( kfdata, &iter->second );
            }
        }

        // 集合
        {
            auto pbchild = &proto_object->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = object_data->Find( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                SyncAddRecordFormServer( kfdata, &iter->second );
            }
        }
    }

    void KFEntityEx::SyncAddRecordFormServer( DataPtr kfrecord, const KFMsg::PBRecord* pbrecord )
    {
        auto pbchild = &pbrecord->proto_object();
        for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
        {
            auto kfdata = CreateData( kfrecord );
            auto ok = _kf_kernel->ParseFromProto( kfdata, &iter->second );
            if ( ok )
            {
                // 不存在列表时, 才更新
                if ( kfrecord->Find( kfdata->GetKeyID() ) == nullptr )
                {
                    AddRecord( kfrecord, iter->first, kfdata );
                }
            }
        }
    }

    void KFEntityEx::SyncRemoveDataFromServer( DataPtr object_data, const KFMsg::PBObject* proto_object )
    {
        // object
        {
            auto pbchild = &proto_object->proto_object();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = object_data->Find( iter->first );
                if ( kfdata != nullptr )
                {
                    SyncRemoveDataFromServer( kfdata, &iter->second );
                }
            }
        }

        // record
        {
            auto pbchild = &proto_object->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfrecord = object_data->Find( iter->first );
                if ( kfrecord == nullptr )
                {
                    continue;
                }

                auto pbchildobject = &iter->second.proto_object();
                for ( auto citer = pbchildobject->begin(); citer != pbchildobject->end(); ++citer )
                {
                    RemoveRecord( kfrecord, citer->first );
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFEntityEx::GetConfigValue( const std::string& name, uint64 id, uint64 maxvalue )
    {
        auto kffunction = _kf_component->_get_config_value_function.Find( name );
        if ( kffunction != nullptr )
        {
            return kffunction->CallEx<uint64>( this, id, maxvalue );
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
        auto kfbasic = Find( __STRING__( basic ) );

        UpdateObject( kfbasic, __STRING__( status ), KFEnum::Set, status );
        UpdateObject( kfbasic, __STRING__( statustime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
    }
}