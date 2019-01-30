#include "KFEntityEx.hpp"
#include "KFComponentEx.hpp"
#include "KFKernelConfig.hpp"
#include "KFKernelModule.hpp"
#include "KFProtocol/KFProtocol.h"
#include "KFCore/KFDataFactory.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    KFEntityEx::KFEntityEx()
    {
        _is_inited = false;
        _kf_object = nullptr;
        _kf_component = nullptr;
        _is_need_to_save = false;
        _have_add_pb_object = false;
        _have_remove_pb_object = false;
        _have_update_pb_object = false;
    }

    KFEntityEx::~KFEntityEx()
    {
        KFDataFactory::Release( _kf_object );
    }

    KFData* KFEntityEx::GetData()
    {
        return _kf_object;
    }

    void KFEntityEx::SetKeyID( uint64 id )
    {
        _kf_object->SetKeyID( id );
    }

    uint64 KFEntityEx::GetKeyID()
    {
        return _kf_object->GetKeyID();
    }

    void KFEntityEx::SetName( const std::string& name )
    {
        _name = name;
    }

    const char* KFEntityEx::GetName()
    {
        return _name.c_str();
    }

    // 是否初始化完成
    bool KFEntityEx::IsInited()
    {
        return _is_inited;
    }

    void KFEntityEx::SetInited()
    {
        _is_inited = true;
    }

    bool KFEntityEx::IsNeedToSave()
    {
        return _is_need_to_save;
    }

    void KFEntityEx::SetNeetToSave( bool needtosave )
    {
        _is_need_to_save = needtosave;
    }

    void KFEntityEx::InitData( KFComponentEx* kfcomponent, const std::string& dataname )
    {
        _kf_component = kfcomponent;
        _kf_object = KFDataFactory::CreateData( "Global", dataname );
    }

    KFData* KFEntityEx::CreateData( const std::string& dataname )
    {
        auto kfdata = _kf_object->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return KFDataFactory::CreateData( kfdata->GetDataSetting() );
    }

    KFData* KFEntityEx::CreateData( const std::string& dataname, uint64 key )
    {
        auto kfdata = CreateData( dataname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        kfdata->SetKeyID( key );
        return kfdata;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::UpdateData( KFData* kfdata, const std::string& value )
    {
        kfdata->SetValue<std::string>( value );

        // 属性更新回调
        _kf_component->UpdateDataCallBack( this, kfdata, value );
    }

    void KFEntityEx::UpdateData( KFData* kfparent, const std::string& dataname, const std::string& value )
    {
        auto kfdata = kfparent->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return;
        }

        UpdateData( kfdata, value );
    }

    void KFEntityEx::UpdateData( const std::string& dataname, const std::string& value )
    {
        auto kfdata = _kf_object->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return;
        }

        UpdateData( kfdata, value );
    }


    void KFEntityEx::UpdateData( const std::string& parentname, const std::string& dataname, const std::string& value )
    {
        auto kfdata = _kf_object->FindData( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return;
        }

        UpdateData( kfdata, value );
    }

    void KFEntityEx::UpdateData( const std::string& parentname, uint64 key, const std::string& dataname, const std::string& value )
    {
        auto kfdata = _kf_object->FindData( parentname, key );
        if ( kfdata == nullptr )
        {
            return;
        }

        UpdateData( kfdata, value );
    }

    uint64 KFEntityEx::UpdateData( const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfdata = _kf_object->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return UpdateData( kfdata, _invalid_int, operate, value );
    }

    uint64 KFEntityEx::UpdateData( const std::string& parentname, const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfdata = _kf_object->FindData( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return UpdateData( kfdata, _invalid_int, operate, value );
    }

    uint64 KFEntityEx::UpdateData( const std::string& parentname, uint64 key, const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfparent = _kf_object->FindData( parentname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        return UpdateData( kfparent, key, dataname, operate, value );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfparent, const std::string& dataname, uint32 operate, uint64 value )
    {
        auto kfdata = kfparent->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        auto key = kfparent->GetKeyID();
        return UpdateData( kfdata, key, operate, value );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfparent, uint64 key, const std::string& dataname, uint32 operate, uint64 value )
    {
        // 不存在, 创建
        auto kfobject = kfparent->FindData( key );
        if ( kfobject == nullptr )
        {
            kfobject = KFDataFactory::CreateData( kfparent->GetDataSetting() );
            kfobject->OperateValue( dataname, operate, value );
            AddData( kfparent, key, kfobject );
            return value;
        }

        // 存在就更新
        auto kfdata = kfobject->FindData( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return UpdateData( kfdata, key, operate, value );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfdata, uint32 operate, uint64 value )
    {
        auto key = kfdata->GetKeyID();
        return UpdateData( kfdata, key, operate, value );
    }

    uint64 KFEntityEx::UpdateData( KFData* kfdata, uint64 key, uint32 operate, uint64 value )
    {
        auto oldvalue = kfdata->GetValue< uint64 >();
        auto newvalue = KFUtility::Operate< uint64 >( operate, oldvalue, value );
        if ( oldvalue != newvalue )
        {
            kfdata->SetValue< uint64 >( newvalue );

            // 属性更新回调
            _kf_component->UpdateDataCallBack( this, kfdata, key, operate, value, oldvalue, newvalue );
        }

        return newvalue;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::AddData( const std::string& parentname, uint64 key, KFData* kfdata )
    {
        auto kfparent = _kf_object->FindData( parentname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        return AddData( kfparent, key, kfdata );
    }


    bool KFEntityEx::AddData( KFData* kfparent, uint64 key, KFData* kfdata )
    {
        bool result = kfparent->AddData( key, kfdata );
        if ( result )
        {
            // 添加属性回调
            _kf_component->AddDataCallBack( this, kfparent, key, kfdata );
        }

        return result;
    }

    bool KFEntityEx::AddData( KFData* kfparent, KFData* kfdata )
    {
        auto key = kfdata->GetKeyID();
        return AddData( kfparent, key, kfdata );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFEntityEx::RemoveData( const std::string& parentname, uint64 key )
    {
        auto kfparent = _kf_object->FindData( parentname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        return RemoveData( kfparent, key );
    }

    bool KFEntityEx::RemoveData( KFData* kfparent, uint64 key )
    {
        auto kfdata = kfparent->FindData( key );
        if ( kfdata == nullptr )
        {
            return false;
        }

        // 属性删除回调
        _kf_component->RemoveDataCallBack( this, kfparent, key, kfdata );

        return kfparent->RemoveData( key );
    }

    bool KFEntityEx::RemoveData( const std::string& parentname )
    {
        auto kfparent = _kf_object->FindData( parentname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        std::list<uint64> keyvector;
        auto kfdata = kfparent->FirstData();
        while ( kfdata != nullptr )
        {
            keyvector.push_back( kfdata->GetKeyID() );
            kfdata = kfparent->NextData();
        }

        for ( auto key : keyvector )
        {
            RemoveData( kfparent, key );
        }

        return true;
    }

    bool KFEntityEx::RemoveData( const std::string& parentname, const std::string& dataname )
    {
        auto kfparent = _kf_object->FindData( parentname, dataname );
        if ( kfparent == nullptr )
        {
            return false;
        }

        std::list<uint64> keyvector;
        auto kfdata = kfparent->FirstData();
        while ( kfdata != nullptr )
        {
            keyvector.push_back( kfdata->GetKeyID() );
            kfdata = kfparent->NextData();
        }

        for ( auto key : keyvector )
        {
            RemoveData( kfparent, key );
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::AddElement( const char* function, uint32 line, const KFElements* kfelements, bool showclient, float multiple /* = 1.0f */ )
    {
        if ( kfelements->_element_list.empty() )
        {
            return;
        }

        for ( auto& kfelement : kfelements->_element_list )
        {
            AddElement( function, line, kfelement, multiple );
        }

        auto& strdata = kfelements->Serialize( multiple );
        __LOG_INFO_FUNCTION__( function, line, "entity={} add element=[{}][{}]!", GetKeyID(), multiple, strdata );

        // 显示给客户端
        if ( _kf_component->_show_element_function != nullptr && showclient )
        {
            _kf_component->_show_element_function( this, strdata );
        }
    }

    // 添加元数据
    void KFEntityEx::AddElement( const char* function, uint32 line, const KFElement* kfelement, float multiple )
    {
        auto kfdata = _kf_object->FindData( kfelement->_data_name );
        if ( kfdata == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "can't find data[{}]! ", kfelement->_parent->_data );
        }

        // 如果有注册的特殊处理函数
        auto kffunction = _kf_component->_add_element_function.Find( kfelement->_data_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( function, line, this, kfdata, const_cast< KFElement* >( kfelement ), multiple );
        }

        // 没有注册的函数
        switch ( kfdata->GetType() )
        {
        case KFDataDefine::Type_Object:
            AddObjectElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        case KFDataDefine::Type_Record:
            AddRecordElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        case KFDataDefine::Type_Int32:
        case KFDataDefine::Type_UInt32:
        case KFDataDefine::Type_Int64:
        case KFDataDefine::Type_UInt64:
        case KFDataDefine::Type_Double:
            AddNormalElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        }
    }

    void KFEntityEx::AddNormalElement( const char* function, uint32 line, KFData* kfdata, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsValue() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value!", kfelement->_parent->_data );
        }

        auto kfelementvalue = reinterpret_cast< KFElementVale* >( kfelement );
        auto value = kfelementvalue->CalcValue( multiple );

        // 更新数据
        UpdateData( kfdata, 0, kfelementvalue->_operate, value );
    }

    void KFEntityEx::AddObjectElement( const char* function, uint32 line, KFData* kfdata, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );

        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto& name = iter.first;
            auto value = iter.second->CalcValue( multiple );
            UpdateData( kfdata, name, kfelementobject->_operate, value );
        }
    }

    void KFEntityEx::AddRecordElement( const char* function, uint32 line, KFData* kfdata, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_parent->_data );
        }

        auto kfchild = kfdata->FindData( kfelementobject->_config_id );
        if ( kfchild == nullptr )
        {
            // 不存在, 创建一个, 并更新属性
            kfchild = KFDataFactory::CreateData( kfdata->GetDataSetting() );
            for ( auto& iter : kfelementobject->_values._objects )
            {
                auto& name = iter.first;
                auto value = iter.second->CalcValue( multiple );
                kfchild->OperateValue( name, kfelementobject->_operate, value );
            }

            AddData( kfdata, kfelementobject->_config_id, kfchild );
        }
        else
        {
            // 存在直接更新属性
            for ( auto& iter : kfelementobject->_values._objects )
            {
                auto& name = iter.first;
                auto value = iter.second->CalcValue( multiple );
                UpdateData( kfchild, name, kfelementobject->_operate, value );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 判断元数据是否满足条件
    bool KFEntityEx::CheckElement( const char* function, uint32 line, const KFElements* kfelements, float multiple /* = 1.0f */ )
    {
        for ( auto& kfelement : kfelements->_element_list )
        {
            if ( !CheckElement( function, line, kfelement, multiple ) )
            {
                return false;
            }
        }

        return true;
    }

    bool KFEntityEx::CheckElement( const char* function, uint32 line, const KFElement* kfelement, float multiple )
    {
        auto kfdata = _kf_object->FindData( kfelement->_data_name );
        if ( kfdata == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "can't find data[{}]! ", kfelement->_parent->_data );
            return false;
        }

        // 如果有注册函数, 执行注册函数
        auto kffunction = _kf_component->_check_element_function.Find( kfelement->_data_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( function, line, this, kfdata, const_cast< KFElement* >( kfelement ), multiple  );
        }

        // 找不到处理函数, 用基础函数来处理
        switch ( kfdata->GetType() )
        {
        case KFDataDefine::Type_Record:
            return CheckRecordElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        case KFDataDefine::Type_Object:
            return CheckObjectElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        case KFDataDefine::Type_Int32:
        case KFDataDefine::Type_UInt32:
        case KFDataDefine::Type_Int64:
        case KFDataDefine::Type_UInt64:
        case KFDataDefine::Type_Double:
            return CheckNormalElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        }

        return false;
    }

    bool KFEntityEx::CheckNormalElement( const char* function, uint32 line, KFData* kfdata, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsValue() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value!", kfelement->_parent->_data );
            return false;
        }

        auto kfelementvalue = reinterpret_cast< KFElementVale* >( kfelement );

        auto datavalue = kfdata->GetValue();
        auto elementvalue = kfelementvalue->_value.CalcValue( multiple );
        return datavalue >= elementvalue;
    }

    bool KFEntityEx::CheckObjectElement( const char* function, uint32 line, KFData* kfdata, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );

        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto kfchild = kfdata->FindData( iter.first );
            if ( kfchild == nullptr )
            {
                return false;
            }

            auto datavalue = kfchild->GetValue();
            auto elementvalue = iter.second->CalcValue( multiple );
            if ( datavalue < elementvalue )
            {
                return false;
            }
        }

        return true;
    }

    bool KFEntityEx::CheckRecordElement( const char* function, uint32 line, KFData* kfparent, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_parent->_data );
            return false;
        }

        auto kfdata = kfparent->FindData( kfelementobject->_config_id );
        if ( kfdata == nullptr )
        {
            return false;
        }

        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto kfchild = kfdata->FindData( iter.first );
            if ( kfchild == nullptr )
            {
                return false;
            }

            auto datavalue = kfchild->GetValue();
            auto elementvalue = iter.second->CalcValue( multiple );
            if ( datavalue < elementvalue )
            {
                return false;
            }
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::RemoveElement( const char* function, uint32 line, const KFElements* kfelements, float multiple /* = 1.0f */ )
    {
        for ( auto& kfelement : kfelements->_element_list )
        {
            RemoveElement( function, line, kfelement, multiple );
        }

        auto& strdata = kfelements->Serialize( multiple );
        __LOG_INFO_FUNCTION__( function, line, "entity={} remove element=[{}][{}]!", GetKeyID(), multiple, strdata );
    }

    // 删除元数据
    void KFEntityEx::RemoveElement( const char* function, uint32 line, const KFElement* kfelement, float multiple )
    {
        auto kfdata = _kf_object->FindData( kfelement->_data_name );
        if ( kfdata == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "can't find data[{}]! ", kfelement->_parent->_data );
        }

        // 如果有注册函数, 执行注册函数
        auto kffunction = _kf_component->_remove_element_function.Find( kfelement->_data_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( function, line, this, kfdata, const_cast< KFElement* >( kfelement ), multiple );
        }

        // 找不到处理函数, 用基础函数来处理
        switch ( kfdata->GetType() )
        {
        case KFDataDefine::Type_Record:
            RemoveRecordElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        case KFDataDefine::Type_Object:
            RemoveObjectElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        case KFDataDefine::Type_Int32:
        case KFDataDefine::Type_UInt32:
        case KFDataDefine::Type_Int64:
        case KFDataDefine::Type_UInt64:
        case KFDataDefine::Type_Double:
            RemoveNormalElement( function, line, kfdata, const_cast< KFElement* >( kfelement ), multiple );
            break;
        }
    }

    void KFEntityEx::RemoveNormalElement( const char* function, uint32 line, KFData* kfdata, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsValue() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not value!", kfelement->_parent->_data );
        }

        auto kfelementvalue = reinterpret_cast< KFElementVale* >( kfelement );
        auto value = kfelementvalue->_value.CalcValue( multiple );
        UpdateData( kfdata, 0, KFOperateEnum::Dec, value );
    }

    void KFEntityEx::RemoveObjectElement( const char* function, uint32 line, KFData* kfdata, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto& name = iter.first;
            auto value = iter.second->GetValue( multiple );
            UpdateData( kfdata, name, KFOperateEnum::Dec, value );
        }
    }

    void KFEntityEx::RemoveRecordElement( const char* function, uint32 line, KFData* kfparent, KFElement* kfelement, float multiple )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_parent->_data );
        }

        auto kfdata = kfparent->FindData( kfelementobject->_config_id );
        if ( kfdata == nullptr )
        {
            return;
        }

        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto& name = iter.first;
            auto value = iter.second->GetValue( multiple );
            UpdateData( kfdata, name, KFOperateEnum::Dec, value );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////

#define __FIND_PROTO_OBJECT__\
    auto savedata = datahierarchy.front();\
    datahierarchy.pop_front();\
    if ( savedata->GetType() == KFDataDefine::Type_Object )\
    {\
        pbobject = &( ( *pbobject->mutable_pbobject() )[ savedata->GetName() ] );\
    }\
    else if ( savedata->GetType() == KFDataDefine::Type_Record )\
    {\
        auto pbrecord = &( ( *pbobject->mutable_pbrecord() )[ savedata->GetName() ] );\
        savedata = datahierarchy.front();\
        datahierarchy.pop_front(); \
        pbobject = &( ( *pbrecord->mutable_pbobject() )[ savedata->GetKeyID() ] ); \
    }\

    void KFEntityEx::SyncAddData( KFData* kfdata, uint64 key )
    {
        // 不需要同步
        if ( _kf_component->_entity_sync_add_function == nullptr ||
                !kfdata->HaveFlagMask( KFDataDefine::Mask_Sync_Client ) )
        {
            return;
        }

        // 先遍历父节点
        std::list< KFData* > datahierarchy;
        datahierarchy.push_front( kfdata );

        auto kfparent = kfdata->GetParent();
        while ( kfparent != nullptr )
        {
            datahierarchy.push_front( kfparent );
            kfparent = kfparent->GetParent();
        }

        // 根据层级来确定属性的位置
        // 先pop player这一层
        datahierarchy.pop_front();

        auto pbobject = &_add_pb_object;
        do
        {
            __FIND_PROTO_OBJECT__;
            if ( savedata == kfdata )
            {
                _kf_component->_kf_kernel_module->SaveToObject( savedata, pbobject, KFDataDefine::Mask_Sync_Client );
            }
        } while ( !datahierarchy.empty() );

        // 设置有添加属性
        _have_add_pb_object = true;
    }

    void KFEntityEx::SyncRemoveData( KFData* kfdata, uint64 key )
    {
        // 不需要同步
        if ( _kf_component->_entity_sync_remove_function == nullptr ||
                !kfdata->HaveFlagMask( KFDataDefine::Mask_Sync_Client ) )
        {
            return;
        }

        // 先遍历父节点
        std::list< KFData* > datahierarchy;
        datahierarchy.push_front( kfdata );

        auto kfparent = kfdata->GetParent();
        while ( kfparent != nullptr )
        {
            datahierarchy.push_front( kfparent );
            kfparent = kfparent->GetParent();
        }

        // 根据层级来确定属性的位置
        // 先pop player这一层
        datahierarchy.pop_front();

        auto pbobject = &_remove_pb_object;
        do
        {
            __FIND_PROTO_OBJECT__;
        } while ( !datahierarchy.empty() );

        _have_remove_pb_object = true;
    }

    void KFEntityEx::SyncUpdateData( KFData* kfdata, uint64 key )
    {
        // 不需要同步
        if ( _kf_component->_entity_sync_update_function == nullptr ||
                !kfdata->HaveFlagMask( KFDataDefine::Mask_Sync_Client ) )
        {
            return;
        }

        // 先遍历父节点
        std::list< KFData* > datahierarchy;
        datahierarchy.push_front( kfdata );

        auto kfparent = kfdata->GetParent();
        while ( kfparent != nullptr )
        {
            datahierarchy.push_front( kfparent );
            kfparent = kfparent->GetParent();
        }

        // 根据层级来确定属性的位置
        // 先pop player这一层
        datahierarchy.pop_front();
        if ( datahierarchy.empty() )
        {
            return;
        }

        auto pbobject = &_update_pb_object;
        do
        {
            __FIND_PROTO_OBJECT__;
            if ( savedata == kfdata )
            {
                AddSyncUpdateDataToPBObject( savedata, pbobject );
            }
        } while ( !datahierarchy.empty() );

        // 设置有添加属性
        _have_update_pb_object = true;
    }

    void KFEntityEx::AddSyncUpdateDataToPBObject( KFData* kfdata, KFMsg::PBObject* pbobject )
    {
        auto datasetting = kfdata->GetDataSetting();
        switch ( datasetting->_type )
        {
        case KFDataDefine::Type_Int32:
            ( *pbobject->mutable_pbint32() )[ datasetting->_name ] = kfdata->GetValue< int32 >();
            break;
        case KFDataDefine::Type_UInt32:
            ( *pbobject->mutable_pbuint32() )[ datasetting->_name ] = kfdata->GetValue< uint32 >();
            break;
        case KFDataDefine::Type_Int64:
            ( *pbobject->mutable_pbint64() )[ datasetting->_name ] = kfdata->GetValue< int64 >();
            break;
        case KFDataDefine::Type_UInt64:
            ( *pbobject->mutable_pbuint64() )[ datasetting->_name ] = kfdata->GetValue< uint64 >();
            break;
        case KFDataDefine::Type_Double:
            ( *pbobject->mutable_pbdouble() )[ datasetting->_name ] = kfdata->GetValue< double >();
            break;
        case KFDataDefine::Type_String:
            ( *pbobject->mutable_pbstring() )[ datasetting->_name ] = kfdata->GetValue< std::string >();
            break;
        case KFDataDefine::Type_Array:
            break;
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    void KFEntityEx::SyncEntityToClient()
    {
        // 同步删除
        SyncRemoveDataToClient();

        // 同步添加
        SyncAddDataToClient();

        // 同步更新
        SyncUpdateDataToClient();
    }

    void KFEntityEx::SyncAddDataToClient()
    {
        if ( !_have_add_pb_object )
        {
            return;
        }

        _kf_component->_entity_sync_add_function( this, _add_pb_object );
        _have_add_pb_object = false;
        _add_pb_object.Clear();
    }

    void KFEntityEx::SyncUpdateDataToClient()
    {
        if ( !_have_update_pb_object )
        {
            return;
        }

        _kf_component->_entity_sync_update_function( this, _update_pb_object );
        _have_update_pb_object = false;
        _update_pb_object.Clear();
    }

    void KFEntityEx::SyncRemoveDataToClient()
    {
        if ( !_have_remove_pb_object )
        {
            return;
        }

        _kf_component->_entity_sync_remove_function( this, _remove_pb_object );
        _have_remove_pb_object = false;
        _remove_pb_object.Clear();
    }
}