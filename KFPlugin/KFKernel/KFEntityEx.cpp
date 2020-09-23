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

    KFData* KFEntityEx::CreateData( const std::string& dataname )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return CreateData( kfdata );
    }

    KFData* KFEntityEx::CreateData( KFData* kfdata )
    {
        auto child = KFDataFactory::Instance()->CreateData( kfdata->_data_setting, true );
        return child;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::UpdateData( KFData* kfdata, const std::string& value, bool callback )
    {
        kfdata->Set<std::string>( value );
        if ( kfdata->IsArray() )
        {
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
            _kf_component->UpdateDataCallBack( this, kfdata, oldvalue, value, callback );
        }
    }

    void KFEntityEx::UpdateData( const std::string& dataname, const std::string& value, bool callback )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, dataname );
        }

        UpdateData( kfdata, value, callback );
    }

    void KFEntityEx::UpdateObject( KFData* kfparent, const std::string& dataname, const std::string& value, bool callback )
    {
        auto kfdata = kfparent->Find( dataname );
        if ( kfdata == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", kfparent->_data_setting->_name, dataname );
        }

        UpdateData( kfdata, value, callback );
    }

    void KFEntityEx::UpdateObject( const std::string& parentname, const std::string& dataname, const std::string& value, bool callback )
    {
        auto kfparent = Find( parentname );
        if ( kfparent == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parentname );
        }

        UpdateObject( kfparent, dataname, value, callback );
    }

    void KFEntityEx::UpdateRecord( const std::string& parentname, uint64 key, const std::string& dataname, const std::string& value, bool callback )
    {
        auto kfparent = Find( parentname );
        if ( kfparent == nullptr )
        {
            return __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parentname );
        }

        auto kfobject = kfparent->Find( key );
        if ( kfobject == nullptr )
        {
            return __LOG_ERROR__( "record=[{}] not have object=[{}]", parentname, key );
        }

        UpdateObject( kfobject, dataname, value, callback );
    }

    uint64 KFEntityEx::UpdateData( const std::string& dataname, uint32 operate, uint64 value, bool callback )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, dataname );
            return _invalid_int;
        }

        return UpdateData( _invalid_int, kfdata, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfdata, uint32 operate, uint64 value, bool callback )
    {
        return UpdateData( _invalid_int, kfdata, operate, value );
    }

    uint64 KFEntityEx::UpdateData( uint64 key, KFData* kfdata, uint32 operate, uint64 value, bool callback )
    {
        if ( !kfdata->IsInt() )
        {
            __LOG_ERROR__( "data=[{}] is not int=[{}]", kfdata->_data_setting->_name );
            return _invalid_int;
        }

        auto oldvalue = kfdata->Get< uint64 >();
        auto newvalue = kfdata->Operate( operate, value );
        _kf_component->UpdateDataCallBack( this, key, kfdata, _invalid_int, operate, value, oldvalue, newvalue, callback );

        return newvalue;
    }

    uint64 KFEntityEx::UpdateObject( const std::string& parentname, const std::string& dataname, uint32 operate, uint64 value, bool callback )
    {
        auto kfparent = Find( parentname );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have object=[{}]", _data_setting->_name, parentname );
            return _invalid_int;
        }

        return UpdateObject( kfparent, dataname, operate, value );
    }

    uint64 KFEntityEx::UpdateObject( KFData* kfparent, const std::string& dataname, uint32 operate, uint64 value, bool callback )
    {
        auto kfdata = kfparent->Find( dataname );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", kfparent->_data_setting->_name, dataname );
            return _invalid_int;
        }

        return UpdateData( kfparent->GetKeyID(), kfdata, operate, value );
    }

    uint64 KFEntityEx::UpdateRecord( const std::string& parentname, uint64 key, const std::string& dataname, uint32 operate, uint64 value, bool callback )
    {
        auto kfparent = Find( parentname );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, parentname );
            return _invalid_int;
        }

        return UpdateRecord( kfparent, key, dataname, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateRecord( KFData* kfparent, uint64 key, const std::string& dataname, uint32 operate, uint64 value, bool callback )
    {
        // 不存在, 创建
        auto kfobject = kfparent->Find( key );
        if ( kfobject == nullptr )
        {
            kfobject = CreateData( kfparent );
            value = kfobject->Operate( dataname, operate, value );
            AddRecord( kfparent, key, kfobject, callback );
            return value;
        }

        // 存在就更新
        auto kfdata = kfobject->Find( dataname );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "object=[{}] not have data=[{}]", kfobject->_data_setting->_name, dataname );
            return _invalid_int;
        }

        return UpdateData( key, kfdata, operate, value, callback );
    }

    uint64 KFEntityEx::UpdateArray( const std::string& dataname, uint64 index, uint32 operate, uint64 value, bool callback )
    {
        auto kfarray = Find( dataname );
        if ( kfarray == nullptr )
        {
            __LOG_ERROR__( "object=[{}] not have array=[{}]", _data_setting->_name, dataname );
            return _invalid_int;
        }

        if ( !kfarray->IsArray() )
        {
            __LOG_ERROR__( "data=[{}] is not array", dataname );
            return _invalid_int;
        }

        return UpdateObjectArray( index, kfarray, index, operate, value );
    }

    uint64 KFEntityEx::UpdateArray( KFData* kfarray, uint64 index, uint32 operate, uint64 value, bool callback )
    {
        return UpdateObjectArray( index, kfarray, index, operate, value );
    }

    uint64 KFEntityEx::UpdateObjectArray( uint64 key, KFData* kfarray, uint64 index, uint32 operate, uint64 value, bool callback )
    {
        auto kfdata = kfarray->Find( index );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "array=[{}] not index=[{}] max=[{}]", kfarray->_data_setting->_name, index, kfarray->MaxSize() );
            return _invalid_int;
        }

        auto oldvalue = kfdata->Get< uint64 >();
        auto newvalue = kfdata->Operate( operate, value );
        _kf_component->UpdateDataCallBack( this, key, kfdata, index, operate, value, oldvalue, newvalue, callback );

        return newvalue;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::AddRecord( const std::string& parentname, uint64 key, KFData* kfdata, bool callback )
    {
        auto kfparent = Find( parentname );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parentname );
            return false;
        }

        if ( !kfparent->IsRecord() )
        {
            __LOG_ERROR__( "data=[{}] is not record", parentname );
            return false;
        }

        if ( kfdata->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", kfdata->_data_setting->_name );
            return false;
        }

        return AddRecord( kfparent, key, kfdata, callback );
    }

    bool KFEntityEx::AddRecord( KFData* kfparent, KFData* kfdata, bool callback )
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

    bool KFEntityEx::AddRecord( KFData* kfparent, uint64 key, KFData* kfdata, bool callback )
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
    void KFEntityEx::InitArray( KFData* kfarray, uint32 size )
    {
        KFDataFactory::Instance()->InitArray( kfarray, size );
    }

    KFData* KFEntityEx::AddArray( KFData* kfarray, int64 value )
    {
        auto kfdata = KFDataFactory::Instance()->AddArray( kfarray );
        kfdata->Set( value );
        return kfdata;
    }

#define __ADD_ARRAY_DATA__( type )\
    void KFEntityEx::AddArray( KFData* kfdata, const type& inlist )\
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
    void KFEntityEx::AddObjectArray( KFData* kfdata, const std::string& dataname, const type& inlist )\
    {\
        if ( kfdata->IsObject() )\
        {\
            return __LOG_ERROR__( "data=[{}] is not object", kfdata->_data_setting->_name );\
        }\
        auto kfarray = kfdata->Find( dataname );\
        if ( kfarray == nullptr )\
        {\
            return __LOG_ERROR__( "parent=[{}] not have data=[{}]", kfdata->_data_setting->_name, dataname );\
        }\
        AddArray( kfarray, inlist );\
    }\

    __ADD_ARRAY_DATA__( UInt32Set );
    __ADD_ARRAY_DATA__( UInt32List );
    __ADD_ARRAY_DATA__( UInt32Vector );

    __ADD_ARRAY_DATA_1__( UInt32Set );
    __ADD_ARRAY_DATA_1__( UInt32List );
    __ADD_ARRAY_DATA_1__( UInt32Vector );
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::RemoveRecord( const std::string& parentname, uint64 key, bool callback )
    {
        auto kfparent = Find( parentname );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parentname );
            return false;
        }

        return RemoveRecord( kfparent, key, callback );
    }

    bool KFEntityEx::RemoveRecord( KFData* kfparent, uint64 key, bool callback )
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

        // 属性删除回调
        _kf_component->RemoveDataCallBack( this, kfparent, key, kfdata, callback );

        KFDataFactory::Instance()->DestroyData( kfdata );
        return kfparent->Move( key );
    }

    bool KFEntityEx::ClearRecord( const std::string& parentname, bool callback )
    {
        auto kfparent = Find( parentname );
        if ( kfparent == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", _data_setting->_name, parentname );
            return false;
        }

        return ClearRecord( kfparent, callback );
    }

    bool KFEntityEx::ClearRecord( KFData* kfparent, bool callback )
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

    bool KFEntityEx::RemoveObject( KFData* kfparent, const std::string& dataname, bool callback )
    {
        if ( !kfparent->IsObject() )
        {
            __LOG_ERROR__( "data=[{}] is not object", kfparent->_data_setting->_name );
            return false;
        }

        auto kfdata = kfparent->Find( dataname );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have data=[{}]", kfparent->_data_setting->_name, dataname );
            return false;
        }

        _kf_component->RemoveDataCallBack( this, kfparent, 0, kfdata, callback );

        KFDataFactory::Instance()->DestroyData( kfdata );
        return kfparent->Move( dataname, false ) != nullptr;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFEntityEx::MoveRecord( KFData* kfparent, uint64 key, bool callback )
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


    KFData* KFEntityEx::MoveRecordToRecord( const std::string& sourcename, uint64 key, const std::string& targetname )
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

    KFData* KFEntityEx::MoveRecordToRecord( KFData* sourcedata, uint64 key, KFData* targetdata )
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

    KFData* KFEntityEx::MoveObjectToRecord( KFData* sourcedata, const std::string& dataname, const std::string& targetname )
    {
        auto targetdata = Find( targetname );
        if ( targetdata == nullptr )
        {
            __LOG_ERROR__( "parent=[{}] not have record=[{}]", _data_setting->_name, targetname );
            return nullptr;
        }

        return MoveObjectToRecord( sourcedata, dataname, targetdata );
    }

    KFData* KFEntityEx::MoveObjectToRecord( KFData* sourcedata, const std::string& dataname, KFData* targetdata )
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
        auto kfdata = sourcedata->Move( dataname, true );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR__( "object=[{}] not have data=[{}]", sourcedata->_data_setting->_name, dataname );
            return nullptr;
        }
        _kf_component->RemoveDataCallBack( this, sourcedata, 0, kfdata, false );

        // 添加属性
        auto uuid = kfdata->GetKeyID();
        targetdata->Add( uuid, kfdata );
        _kf_component->AddDataCallBack( this, targetdata, uuid, kfdata, false );

        return kfdata;
    }

    KFData* KFEntityEx::MoveRecordToObject( KFData* sourcedata, uint64 key, KFData* targetdata, const std::string& dataname )
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
        targetdata->Add( dataname, kfdata );
        SyncUpdateDataToClient( kfdata, key );

        return kfdata;
    }

    KFData* KFEntityEx::MoveObjectToObject( KFData* sourcedata, const std::string& sourcename, KFData* targetdata, const std::string& targetname )
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

    KFMsg::PBShowData* KFEntityEx::CreateShowData( KFMsg::PBShowElement* pbshowelement, const std::string& name, uint64 value, bool find, const std::string& extendname )
    {
        if ( find )
        {
            for ( auto i = 0; i < pbshowelement->pbdata_size(); ++i )
            {
                auto pbdata = pbshowelement->mutable_pbdata( i );
                if ( pbdata->name() != name || pbdata->extendname() != extendname )
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
        pbdata->set_extendname( extendname );
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
                CreateShowData( pbshowelement, kfelementvalue->_data_name, kfelementvalue->_value->GetUseValue(), false, _invalid_string );
            }
        }
        else if ( kfelement->IsObject() )
        {
            auto kfelementobject = reinterpret_cast< const KFElementObject* >( kfelement );
            if ( kfelementobject->IsNeedShow() )
            {
                auto pbshowdata = CreateShowData( pbshowelement, kfelementobject->_data_name, kfelementobject->_config_id, false, _invalid_string );
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
            AddDataToShow( kfresult->_module_name, kfresult->_module_id, kfresult->_element->_data_name, kfresult->_total_value, false );
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
            AddDataToShow( kfresult->_module_name, kfresult->_module_id, kfresult->_element->_data_name, kfresult->_total_value, false );
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
                values[ __STRING__( id ) ] = kfresult->_config_id;
                values[ kfresult->_data_name ] = iter.second;
                AddDataToShow( kfresult->_module_name, kfresult->_module_id, kfresult->_element->_data_name, kfresult->_config_id, values, false, iter.first );
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
    void KFEntityEx::AddDataToShow( const std::string& modulename, uint64 moduleid )
    {
        if ( modulename.empty() )
        {
            return;
        }

        CreateShowElement( modulename, moduleid );
    }

    void KFEntityEx::AddDataToShow( const std::string& name, uint64 value, bool find )
    {
        auto pbshowelement = CreateShowElement();
        if ( pbshowelement == nullptr )
        {
            return;
        }

        CreateShowData( pbshowelement, name, value, find, _invalid_string );
    }

    void KFEntityEx::AddDataToShow( const std::string& modulename, uint64 moduleid, const std::string& name, uint64 value, bool find )
    {
        if ( value == 0u )
        {
            return;
        }

        auto pbshowelement = CreateShowElement( modulename, moduleid );
        CreateShowData( pbshowelement, name, value, find, _invalid_string );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddDataToShow( const std::string& name, uint64 value, StringUInt64& values, bool find, const std::string& extendname )
    {
        auto pbshowelement = CreateShowElement();
        if ( pbshowelement == nullptr )
        {
            return;
        }

        AddDataToShow( pbshowelement, name, value, values, find, extendname );
    }

    void KFEntityEx::AddDataToShow( const std::string& modulename, uint64 moduleid, const std::string& name, uint64 value, StringUInt64& values, bool find, const std::string& extendname /* = _invalid_string */ )
    {
        auto pbshowelement = CreateShowElement( modulename, moduleid );
        AddDataToShow( pbshowelement, name, value, values, find, extendname );
    }

    void KFEntityEx::AddDataToShow( KFMsg::PBShowElement* pbshowelement, const std::string& name, uint64 value, StringUInt64& values, bool find, const std::string& extendname )
    {
        auto pbshowdata = CreateShowData( pbshowelement, name, value, find, extendname );
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
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddDataToShow( KFMsg::PBShowElement* pbshowelement, KFData* kfdata )
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
            CreateShowData( pbshowelement, kfdata->_data_setting->_logic_name, kfdata->Get(), true, kfdata->_data_setting->_name );
            break;
        }
        case KFDataDefine::DataTypeObject:
        case KFDataDefine::DataTypeRecord:
        {
            auto configid = kfdata->Get( kfdata->_data_setting->_config_key_name );
            auto pbshowdata = CreateShowData( pbshowelement, kfdata->_data_setting->_logic_name, configid, false, kfdata->_data_setting->_name );
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

    void KFEntityEx::AddDataToShow( const std::string& modulename, uint64 moduleid, KFData* kfdata )
    {
        auto pbshowelement = CreateShowElement( modulename, moduleid );
        AddDataToShow( pbshowelement, kfdata );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFEntityEx::FindByLogicName( const std::string& name )
    {
        auto kfdatasetting = _data_setting->_class_setting->FindSettingByLogicName( name );
        if ( kfdatasetting == nullptr )
        {
            return nullptr;
        }

        return Find( kfdatasetting->_name );
    }

    const std::string& KFEntityEx::CheckAddElement( const KFElements* kfelements, double multiple, const char* function, uint32 line )
    {
        for ( auto kfelement : kfelements->_element_list )
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

    void KFEntityEx::AddElement( const KFElements* kfelements, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        if ( kfelements->_element_list.empty() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "{}=[{}] element=[{}] is empty", _kf_component->_component_name, GetKeyID(), kfelements->GetElement() );
        }

        __LOG_INFO_FUNCTION__( function, line, "{}=[{}] start add elements={} multiple=[{:0.2f}]", _kf_component->_component_name, GetKeyID(), kfelements->GetElement(), multiple );
        _element_sequence = KFGlobal::Instance()->STMakeUuid();
        for ( auto kfelement : kfelements->_element_list )
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

    bool KFEntityEx::AddNormalElement( KFData* kfdata, KFElementResult* kfresult, const char* function, uint32 line )
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

    void KFEntityEx::UpdateElementToData( KFData* kfdata, KFElementObject* kfelement, double multiple )
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

    void KFEntityEx::SetElementToData( KFData* kfdata, KFElementObject* kfelement, double multiple )
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

    bool KFEntityEx::AddObjectElement( KFData* kfparent, KFElementResult* kfresult, const char* function, uint32 line )
    {
        if ( !kfresult->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfresult->_element->_data_name );
            return false;
        }

        UpdateElementToData( kfparent, reinterpret_cast< KFElementObject* >( kfresult->_element ), kfresult->_multiple );
        return true;
    }

    bool KFEntityEx::AddRecordElement( KFData* kfparent, KFElementResult* kfresult, const char* function, uint32 line )
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
    const std::string& KFEntityEx::CheckRemoveElement( const KFElements* kfelements, double multiple, const char* function, uint32 line )
    {
        for ( auto kfelement : kfelements->_element_list )
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

    bool KFEntityEx::CheckNormalElement( KFData* kfdata, KFElement* kfelement, double multiple, const char* function, uint32 line )
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

        auto datavalue = kfdata->Get();
        auto elementvalue = kfelementvalue->_value->CalcUseValue( kfdata->_data_setting, multiple );
        return datavalue >= elementvalue;
    }

    bool KFEntityEx::CheckObjectElement( KFData* kfparent, KFElement* kfelement, double multiple, const char* function, uint32 line )
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

            auto datavalue = kfchild->Get() ;
            auto elementvalue = kfvalue->CalcUseValue( kfchild->_data_setting, multiple );
            if ( datavalue < elementvalue )
            {
                return false;
            }
        }

        return true;
    }

    bool KFEntityEx::CheckRecordElement( KFData* kfparent, KFElement* kfelement, double multiple, const char* function, uint32 line )
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
    const std::string& KFEntityEx::RemoveElement( const KFElements* kfelements, double multiple, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        if ( kfelements->IsEmpty() )
        {
            return _invalid_string;
        }

        auto& dataname = CheckRemoveElement( kfelements, multiple, function, line );
        if ( dataname.empty() )
        {
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] start remove elements={} multiple=[{:0.2f}]", _kf_component->_component_name, GetKeyID(), kfelements->GetElement(), multiple );
            _element_sequence = KFGlobal::Instance()->STMakeUuid();
            for ( auto kfelement : kfelements->_element_list )
            {
                RemoveElement( kfelement, multiple, modulename, moduleid, function, line );
            }
            __LOG_INFO_FUNCTION__( function, line, "{}=[{}] finish remove elements", _kf_component->_component_name, GetKeyID() );
        }

        return dataname;
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

    bool KFEntityEx::RemoveNormalElement( KFData* kfdata, KFElementResult* kfresult, const char* function, uint32 line )
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

    bool KFEntityEx::RemoveObjectElement( KFData* kfobject, KFElementResult* kfresult, const char* function, uint32 line )
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

            UpdateObject( kfobject, name, KFEnum::Dec, kfvalue->GetUseValue() );
        }

        return true;
    }

    bool KFEntityEx::RemoveRecordElement( KFData* kfparent, KFElementResult* kfresult, const char* function, uint32 line )
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

        auto kfobject = kfparent->Find( kfelementobject->_config_id );
        if ( kfobject == nullptr )
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

            UpdateObject( kfobject, name, KFEnum::Dec, kfvalue->GetUseValue() );
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
    std::list< KFData* > datahierarchy;\
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
        pbobject = &( ( *pbobject->mutable_pbobject() )[ savedata->_data_setting->_name ] );\
    }\
    else if ( savedata->_data_type == KFDataDefine::DataTypeRecord )\
    {\
        auto pbrecord = &( ( *pbobject->mutable_pbrecord() )[ savedata->_data_setting->_name ] );\
        savedata = datahierarchy.front();\
        datahierarchy.pop_front(); \
        pbobject = &( ( *pbrecord->mutable_pbobject() )[ savedata->GetKeyID() ] ); \
    }\

    void KFEntityEx::SyncAddDataToClient( KFData* kfdata, uint64 key )
    {
        __PREPARE_SYNC__( _kf_component->_entity_sync_add_function );
        auto pbobject = CreateSyncPBObject( KFEnum::Add );
        do
        {
            __FIND_PROTO_OBJECT__;
            if ( savedata == kfdata )
            {
                static_cast<KFKernelModule*>( _kf_kernel )->SaveToObject( savedata, pbobject, KFDataDefine::DataMaskSync );
            }
        } while ( !datahierarchy.empty() );
    }

    void KFEntityEx::SyncRemoveDataToClient( KFData* kfdata, uint64 key )
    {
        __PREPARE_SYNC__( _kf_component->_entity_sync_remove_function );
        auto pbobject = CreateSyncPBObject( KFEnum::Dec );
        do
        {
            __FIND_PROTO_OBJECT__;
        } while ( !datahierarchy.empty() );
    }

    void KFEntityEx::SynAddRecordDataToClient( KFData* kfdata )
    {
        for ( auto kfchild = kfdata->First(); kfchild != nullptr; kfchild = kfdata->Next() )
        {
            SyncAddDataToClient( kfchild, kfchild->GetKeyID() );
        }
    }

    void KFEntityEx::SyncUpdateDataToClient( KFData* kfdata, uint64 key )
    {
        __PREPARE_SYNC__( _kf_component->_entity_sync_update_function );
        auto pbobject = CreateSyncPBObject( KFEnum::Set );
        do
        {
            __FIND_PROTO_OBJECT__;
            if ( savedata->_data_type == KFDataDefine::DataTypeArray )
            {
                auto pbarray = &( ( *pbobject->mutable_pbarray() )[ savedata->_data_setting->_name ] );
                savedata = datahierarchy.front();
                datahierarchy.pop_front();

                ( *pbarray->mutable_pbuint64() )[ key ] = savedata->Get<int64>();
            }
            else
            {
                if ( savedata == kfdata )
                {
                    AddSyncUpdateDataToPBObject( savedata, pbobject );
                }
            }
        } while ( !datahierarchy.empty() );
    }

    void KFEntityEx::AddSyncUpdateDataToPBObject( KFData* kfdata, KFMsg::PBObject* pbobject )
    {
        auto datasetting = kfdata->_data_setting;
        switch ( kfdata->_data_type )
        {
        case KFDataDefine::DataTypeInt32:
            ( *pbobject->mutable_pbint32() )[ datasetting->_name ] = kfdata->Get< int32 >();
            break;
        case KFDataDefine::DataTypeUInt32:
            ( *pbobject->mutable_pbuint32() )[ datasetting->_name ] = kfdata->Get< uint32 >();
            break;
        case KFDataDefine::DataTypeInt64:
            ( *pbobject->mutable_pbint64() )[ datasetting->_name ] = kfdata->Get< int64 >();
            break;
        case KFDataDefine::DataTypeUInt64:
            ( *pbobject->mutable_pbuint64() )[ datasetting->_name ] = kfdata->Get< uint64 >();
            break;
        case KFDataDefine::DataTypeDouble:
            ( *pbobject->mutable_pbdouble() )[ datasetting->_name ] = kfdata->Get< double >();
            break;
        case KFDataDefine::DataTypeString:
            ( *pbobject->mutable_pbstring() )[ datasetting->_name ] = kfdata->Get< std::string >();
            break;
        case KFDataDefine::DataTypeObject:
            static_cast< KFKernelModule* >( _kf_kernel )->SaveToObject( kfdata, pbobject, KFDataDefine::DataMaskSync );
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
        auto pbdata = &pbobject->pbdata();\
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )\
        {\
            auto kfdata = kfobject->Find( iter->first );\
            if ( kfdata != nullptr )\
            {\
                UpdateData( kfdata, KFEnum::Set, iter->second );\
            }\
        }\
    }

#define __SYNC_UPDATE_STR_DATA__( datatype, pbdata )\
    {\
        auto pbdata = &pbobject->pbdata();\
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )\
        {\
            auto kfdata = kfobject->Find( iter->first );\
            if ( kfdata != nullptr )\
            {\
                UpdateData( kfdata, iter->second );\
            }\
        }\
    }

    void KFEntityEx::SyncUpdateDataFromServer( KFData* kfobject, const KFMsg::PBObject* pbobject )
    {
        __SYNC_UPDATE_INT_DATA__( int32, pbint32 );
        __SYNC_UPDATE_INT_DATA__( uint32, pbuint32 );
        __SYNC_UPDATE_INT_DATA__( int64, pbint64 );
        __SYNC_UPDATE_INT_DATA__( uint64, pbuint64 );
        __SYNC_UPDATE_INT_DATA__( double, pbdouble );
        __SYNC_UPDATE_STR_DATA__( std::string, pbstring );

        // array
        {
            auto pbarray = &pbobject->pbarray();
            for ( auto iter = pbarray->begin(); iter != pbarray->end(); ++iter )
            {
                auto kfdata = kfobject->Find( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                auto pbuint64 = &iter->second.pbuint64();
                for ( auto citer = pbuint64->begin(); citer != pbuint64->end(); ++citer )
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
            auto pbchild = &pbobject->pbobject();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = kfobject->Find( iter->first );
                if ( kfdata != nullptr )
                {
                    SyncUpdateDataFromServer( kfdata, &iter->second );
                }
            }
        }

        // record
        {
            auto pbchild = &pbobject->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfrecord = kfobject->Find( iter->first );
                if ( kfrecord == nullptr )
                {
                    continue;
                }

                auto pbchildobject = &iter->second.pbobject();
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

    void KFEntityEx::SyncAddDataFromServer( KFData* kfobject, const KFMsg::PBObject* pbobject )
    {
        // 对象
        {
            auto pbchild = &pbobject->pbobject();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = kfobject->Find( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                SyncAddDataFromServer( kfdata, &iter->second );
            }
        }

        // 集合
        {
            auto pbchild = &pbobject->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = kfobject->Find( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }

                SyncAddRecordFormServer( kfdata, &iter->second );
            }
        }
    }

    void KFEntityEx::SyncAddRecordFormServer( KFData* kfrecord, const KFMsg::PBRecord* pbrecord )
    {
        auto pbchild = &pbrecord->pbobject();
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

    void KFEntityEx::SyncRemoveDataFromServer( KFData* kfobject, const KFMsg::PBObject* pbobject )
    {
        // object
        {
            auto pbchild = &pbobject->pbobject();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfdata = kfobject->Find( iter->first );
                if ( kfdata != nullptr )
                {
                    SyncRemoveDataFromServer( kfdata, &iter->second );
                }
            }
        }

        // record
        {
            auto pbchild = &pbobject->pbrecord();
            for ( auto iter = pbchild->begin(); iter != pbchild->end(); ++iter )
            {
                auto kfrecord = kfobject->Find( iter->first );
                if ( kfrecord == nullptr )
                {
                    continue;
                }

                auto pbchildobject = &iter->second.pbobject();
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

        auto kfobject = Find( name, id );
        if ( kfobject != nullptr )
        {
            return kfobject->Get<uint64>( kfobject->_data_setting->_value_key_name );
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