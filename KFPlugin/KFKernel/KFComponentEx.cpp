#include "KFComponentEx.hpp"
#include "KFEntityEx.hpp"
#include "KFKernelModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    KFComponentEx::KFComponentEx()
    {
        _entity_initialize_function = nullptr;
        _entity_uninitialize_function = nullptr;
        _entity_run_function = nullptr;
        _entity_after_run_function = nullptr;
        _entity_save_function = nullptr;
        _entity_delete_function = nullptr;

        _entity_sync_add_function = nullptr;
        _entity_sync_update_function = nullptr;
        _entity_sync_remove_function = nullptr;
        _show_element_function = nullptr;
    }

    KFComponentEx::~KFComponentEx()
    {
        _sync_entitys.clear();
    }

    void KFComponentEx::InitEntity( KFEntity* kfentity )
    {
        // 初始化
        if ( _entity_initialize_function != nullptr )
        {
            _entity_initialize_function( kfentity );
        }
    }

    void KFComponentEx::UnInitEntity( KFEntity* kfentity )
    {
        if ( _entity_uninitialize_function != nullptr )
        {
            _entity_uninitialize_function( kfentity );
        }
    }

    void KFComponentEx::Run()
    {
        if ( _entity_run_function != nullptr )
        {
            for ( auto& iter : _entitys._objects )
            {
                auto kfentity = iter.second;
                _entity_run_function( kfentity );
            }
        }
    }

    void KFComponentEx::AfterRun()
    {
        if ( _entity_after_run_function != nullptr )
        {
            for ( auto& iter : _entitys._objects )
            {
                _entity_after_run_function( iter.second );
            }
        }

        if ( !_sync_entitys.empty() )
        {
            for ( auto entity : _sync_entitys )
            {
                static_cast< KFEntityEx* >( entity )->SyncEntityToClient();
            }
            _sync_entitys.clear();
        }
    }

    StringVector& KFComponentEx::GetDataList( const std::string& dataname )
    {
        static StringVector _result;
        _result.clear();

        auto classsetting = _data_setting->_class_setting;
        for ( auto& iter : classsetting->_static_data._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_logic_name == dataname )
            {
                _result.push_back( kfsetting->_name );
            }
        }

        return _result;
    }

    // 获得类配置
    const KFClassSetting* KFComponentEx::FindClassSetting()
    {
        return _data_setting->_class_setting;
    }

    const KFClassSetting* KFComponentEx::FindClassSetting( const std::string& dataname )
    {
        auto kfsetting = _data_setting->_class_setting->FindSetting( dataname );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        return kfsetting->_class_setting;
    }

    const KFDataSetting* KFComponentEx::FindDataSetting()
    {
        return _data_setting;
    }

    const KFDataSetting* KFComponentEx::FindDataSetting( const std::string& dataname )
    {
        return _data_setting->_class_setting->FindSetting( dataname );
    }

    const KFDataSetting* KFComponentEx::FindDataSetting( const std::string& parentname, const std::string& dataname )
    {
        auto kfsetting = _data_setting->_class_setting->FindSetting( parentname );
        if ( kfsetting == nullptr || kfsetting->_class_setting == nullptr )
        {
            return nullptr;
        }

        return kfsetting->_class_setting->FindSetting( dataname );
    }
    /////////////////////////////////////////////////////////////////////////////////////
    KFEntity* KFComponentEx::FirstEntity()
    {
        return _entitys.First();
    }

    KFEntity* KFComponentEx::NextEntity()
    {
        return _entitys.Next();
    }

    KFEntity* KFComponentEx::CreateEntity( uint64 key )
    {
        auto kfentity = _entitys.Find( key );
        if ( kfentity != nullptr )
        {
            return kfentity;
        }

        kfentity = ( KFEntityEx* )KFDataFactory::Instance()->CreateFromDataPool( _data_setting );
        if ( kfentity != nullptr )
        {
            kfentity->SetKeyID( _invalid_int );
        }
        else
        {
            kfentity = __KF_NEW__( KFEntityEx );
            KFDataFactory::Instance()->InitData( kfentity, _data_setting );
        }

        static_cast< KFEntityEx* >( kfentity )->InitData( this );
        AddEntity( key, kfentity );
        return kfentity;
    }

    KFEntity* KFComponentEx::CreateEntity( uint64 key, const KFMsg::PBObject* proto )
    {
        auto kfentity = CreateEntity( key );
        auto ok = KFKernelModule::Instance()->ParseFromProto( kfentity, proto );
        if ( ok )
        {
            kfentity->SetKeyID( key );
            InitEntity( kfentity );
        }
        else
        {
            RemoveEntity( kfentity );
            kfentity = nullptr;
            __LOG_ERROR__( "[{}:{}] create error!", _component_name, key );
        }

        return kfentity;
    }

    void KFComponentEx::AddEntity( uint64 key, KFEntity* kfentity )
    {
        kfentity->SetKeyID( key );
        _entitys.Insert( key, kfentity );
    }

    KFEntity* KFComponentEx::FindEntity( uint64 key )
    {
        return _entitys.Find( key );
    }

    KFEntity* KFComponentEx::FindEntity( uint64 key, const char* function, uint32 line )
    {
        auto kfentity = _entitys.Find( key );
        if ( kfentity == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "can't find entity[{}:{}]!", _component_name, key );
        }

        return kfentity;
    }

    uint32 KFComponentEx::GetEntityCount()
    {
        return _entitys.Size();
    }

    bool KFComponentEx::RemoveEntity( uint64 key )
    {
        auto kfentity = FindEntity( key, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return false;
        }

        return RemoveEntity( kfentity );
    }

    bool KFComponentEx::RemoveEntity( KFEntity* kfentity )
    {
        // 释放资源, 保存
        if ( kfentity->IsInited() )
        {
            UnInitEntity( kfentity );
            DeleteSaveEntity( kfentity );
        }

        _sync_entitys.erase( kfentity );

        KFDataFactory::Instance()->DestroyData( kfentity );
        return _entitys.Remove( kfentity->GetKeyID(), false );
    }

    void KFComponentEx::AddSyncEntity( KFEntity* entity )
    {
        _sync_entitys.insert( entity );
    }

    void KFComponentEx::SetLogicValue( const std::string& dataname, uint32 value )
    {
        auto kfsetting = FindDataSetting( dataname );
        const_cast< KFDataSetting* >( kfsetting )->_int_logic_value = value;
    }

    void KFComponentEx::SetLogicValue( const std::string& parentname, const std::string& dataname, uint32 value )
    {
        auto kfsetting = FindDataSetting( parentname, dataname );
        const_cast< KFDataSetting* >( kfsetting )->_int_logic_value = value;
    }

    void KFComponentEx::SetMaxValue( const std::string& dataname, uint32 value )
    {
        auto kfsetting = FindDataSetting( dataname );
        const_cast< KFDataSetting* >( kfsetting )->_int_max_value = value;
    }

    void KFComponentEx::SetMaxValue( const std::string& parentname, const std::string& dataname, uint32 value )
    {
        auto kfsetting = FindDataSetting( parentname, dataname );
        const_cast<KFDataSetting*>( kfsetting )->_int_max_value = value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindCheckAddElementFunction( const std::string& dataname, KFCheckAddElementFunction& function )
    {
        auto kffunction = _check_add_element_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterCheckAddElementFunction( const std::string& dataname )
    {
        _check_add_element_function.Remove( dataname );
    }

    void KFComponentEx::BindAddElementFunction( const std::string& dataname, KFAddElementFunction& function )
    {
        auto kffunction = _add_element_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterAddElementFunction( const std::string& dataname )
    {
        _add_element_function.Remove( dataname );
    }

    void KFComponentEx::BindCheckRemoveElementFunction( const std::string& dataname, KFCheckRemoveElementFunction& function )
    {
        auto kffunction = _check_remove_element_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterCheckRemoveElementFunction( const std::string& dataname )
    {
        _check_remove_element_function.Remove( dataname );
    }

    void KFComponentEx::BindRemoveElementFunction( const std::string& dataname, KFRemoveElementFunction& function )
    {
        auto kffunction = _remove_element_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterRemoveElementFunction( const std::string& dataname )
    {
        _remove_element_function.Remove( dataname );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindAddDataModule( const std::string& module, KFAddDataFunction& function )
    {
        auto kffunction = _add_data_module.Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindAddDataModule( const std::string& module )
    {
        _add_data_module.Remove( module );
    }

    void KFComponentEx::BindAddDataFunction( const std::string& module, const std::string& dataname, uint64 key, KFAddDataFunction& function )
    {
        auto datakey = RecordKeyType( dataname, key );
        auto kfdatafunction = _add_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataFunction( const std::string& module, const std::string& dataname, uint64 key )
    {
        auto datakey = RecordKeyType( dataname, key );
        auto kfdatafunction = _add_data_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindRemoveDataModule( const std::string& module, KFRemoveDataFunction& function )
    {
        auto kffunction = _remove_data_module.Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindRemoveDataModule( const std::string& module )
    {
        _remove_data_module.Remove( module );
    }

    void KFComponentEx::BindRemoveDataFunction( const std::string& module, const std::string& dataname, uint64 key, KFRemoveDataFunction& function )
    {
        auto datakey = RecordKeyType( dataname, key );
        auto kfdatafunction = _remove_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataFunction( const std::string& module, const std::string& dataname, uint64 key )
    {
        auto datakey = RecordKeyType( dataname, key );
        auto kfdatafunction = _remove_data_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindUpdateDataModule( const std::string& module, KFUpdateDataFunction& function )
    {
        auto kffunction = _update_data_module.Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindUpdateDataModule( const std::string& module )
    {
        _update_data_module.Remove( module );
    }

    const DataKeyType& KFComponentEx::FormatDataKey( const std::string& parentname, const std::string& dataname )
    {
        static DataKeyType _data_key;
        if ( parentname.empty() )
        {
            _data_key = DataKeyType( _component_name, dataname );
        }
        else
        {
            _data_key = DataKeyType( parentname, dataname );
        }

        return _data_key;
    }

    const DataKeyType& KFComponentEx::FormatDataKey( KFData* kfdata )
    {
        auto kfparent = kfdata->GetParent();
        if ( kfparent->_data_setting->_type == KFDataDefine::Type_Array )
        {
            if ( kfparent->GetParent() != nullptr )
            {
                return FormatDataKey( kfparent->GetParent()->_data_setting->_logic_name, kfparent->_data_setting->_logic_name );
            }
        }

        return FormatDataKey( kfparent->_data_setting->_logic_name, kfdata->_data_setting->_logic_name );
    }

    void KFComponentEx::BindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname, KFUpdateDataFunction& function )
    {
        auto& datakey = FormatDataKey( parentname, dataname );
        auto kfdatafunction = _update_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname )
    {
        auto& datakey = FormatDataKey( parentname, dataname );
        auto kfdatafunction = _update_data_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindUpdateStringModule( const std::string& module, KFUpdateStringFunction& function )
    {
        auto kffunction = _update_string_module.Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindUpdateStringModule( const std::string& module )
    {
        _update_string_module.Remove( module );
    }

    void KFComponentEx::BindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname, KFUpdateStringFunction& function )
    {
        auto& datakey = FormatDataKey( parentname, dataname );
        auto kfdatafunction = _update_string_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname )
    {
        auto& datakey = FormatDataKey( parentname, dataname );
        auto kfdatafunction = _update_string_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindEntityInitializeFunction( KFEntityFunction& function )
    {
        _entity_initialize_function = function;
    }

    void KFComponentEx::UnRegisterEntityInitializeFunction()
    {
        _entity_initialize_function = nullptr;
    }

    void KFComponentEx::BindEntityUninitializeFunction( KFEntityFunction& function )
    {
        _entity_uninitialize_function = function;
    }

    void KFComponentEx::UnRegisterEntityUninitializeFunction()
    {
        _entity_uninitialize_function = nullptr;
    }

    void KFComponentEx::BindEntityRunFunction( KFEntityFunction& function )
    {
        _entity_run_function = function;
    }

    void KFComponentEx::UnRegisterEntityRunFunction()
    {
        _entity_run_function = nullptr;
    }

    void KFComponentEx::BindEntityAfterRunFunction( KFEntityFunction& function )
    {
        _entity_after_run_function = function;
    }

    void KFComponentEx::UnRegisterEntityAfterRunFunction()
    {
        _entity_after_run_function = nullptr;
    }

    void KFComponentEx::BindEntitySaveFunction( KFSaveEntityFunction& function )
    {
        _entity_save_function = function;
    }

    void KFComponentEx::UnRegisterEntitySaveFunction()
    {
        _entity_save_function = nullptr;
    }

    void KFComponentEx::BindEntityDeleteFunction( KFEntityFunction& function )
    {
        _entity_delete_function = function;
    }

    void KFComponentEx::UnRegisterEntityDeleteFunction()
    {
        _entity_delete_function = nullptr;
    }

    void KFComponentEx::BindSyncAddFunction( KFSyncFunction& function )
    {
        _entity_sync_add_function = function;
    }

    void KFComponentEx::UnRegisterSyncAddFunction()
    {
        _entity_sync_add_function = nullptr;
    }

    void KFComponentEx::BindSyncRemoveFunction( KFSyncFunction& function )
    {
        _entity_sync_remove_function = function;
    }

    void KFComponentEx::UnRegisterSyncRemoveFunction()
    {
        _entity_sync_remove_function = nullptr;
    }

    void KFComponentEx::BindSyncUpdateFunction( KFSyncFunction& function )
    {
        _entity_sync_update_function = function;
    }

    void KFComponentEx::UnRegisterSyncUpdateFunction()
    {
        _entity_sync_update_function = nullptr;
    }

    void KFComponentEx::BindShowElementFunction( KFShowElementFunction& function )
    {
        _show_element_function = function;
    }

    void KFComponentEx::UnRegisterShowElementFunction()
    {
        _show_element_function = nullptr;
    }

    void KFComponentEx::BindGetConfigValueFunction( const std::string& name, KFGetConfigValueFunction& function )
    {
        auto kffunction = _get_config_value_function.Create( name );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindGetConfigValueFunction( const std::string& name )
    {
        _get_config_value_function.Remove( name );
    }

    void KFComponentEx::BindLogElementFunction( const std::string& name, KFLogElementFunction& function )
    {
        auto kffunction = _log_element_function.Create( name );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindLogElementFunction( const std::string& name )
    {
        _log_element_function.Remove( name );
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::UpdateDataCallBack( KFEntity* kfentity, uint64 key, KFData* kfdata, uint64 index, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        // 更新同步
        kfentity->SyncUpdateData( kfdata, index );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( kfdata->HaveMask( KFDataDefine::Mask_UpdataCall ) &&
                kfdata->GetParent()->HaveMask( KFDataDefine::Mask_UpdataCall ) )
        {
            for ( auto& iter : _update_data_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, key, kfdata, operate, value, oldvalue, newvalue );
            }
        }

        // 注册的函数
        auto& findkey = FormatDataKey( kfdata );
        auto kfdatafunction = _update_data_function.Find( findkey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->CallFunction( kfentity, key, kfdata, operate, value, oldvalue, newvalue );
        }
    }

    void KFComponentEx::UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, const std::string& value, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        // 更新同步
        kfentity->SyncUpdateData( kfdata, kfentity->GetKeyID() );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( kfdata->HaveMask( KFDataDefine::Mask_UpdataCall ) &&
                kfdata->GetParent()->HaveMask( KFDataDefine::Mask_UpdataCall ) )
        {
            for ( auto& iter : _update_string_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, kfdata, value );
            }
        }

        // 注册的函数
        auto& findkey = FormatDataKey( kfdata );
        auto kfdatafunction = _update_string_function.Find( findkey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->CallFunction( kfentity, kfdata, value );
        }
    }

    void KFComponentEx::AddDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata, bool callback )
    {
        if ( callback )
        {
            // 模块回调
            if ( kfdata->HaveMask( KFDataDefine::Mask_AddCall ) )
            {
                for ( auto& iter : _add_data_module._objects )
                {
                    auto kffunction = iter.second;
                    kffunction->_function( kfentity, kfparent, key, kfdata );
                }
            }

            // 注册的函数
            {
                auto findkey = RecordKeyType( kfdata->_data_setting->_logic_name, key );
                auto kfdatafunction = _add_data_function.Find( findkey );
                if ( kfdatafunction != nullptr )
                {
                    kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
                }
            }
            {
                auto findkey = RecordKeyType( kfdata->_data_setting->_logic_name, 0u );
                auto kfdatafunction = _add_data_function.Find( findkey );
                if ( kfdatafunction != nullptr )
                {
                    kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
                }
            }
        }

        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );
        kfentity->SyncAddData( kfdata, key );
    }

    void KFComponentEx::RemoveDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        // 同步客户端
        kfentity->SyncRemoveData( kfdata, key );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( kfdata->HaveMask( KFDataDefine::Mask_RemoveCall ) )
        {
            for ( auto& iter : _remove_data_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, kfparent, key, kfdata );
            }
        }

        // 注册的函数
        {
            auto findkey = RecordKeyType( kfdata->_data_setting->_logic_name, key );
            auto kfdatafunction = _remove_data_function.Find( findkey );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
            }
        }
        {
            auto findkey = RecordKeyType( kfdata->_data_setting->_logic_name, 0u );
            auto kfdatafunction = _remove_data_function.Find( findkey );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
            }
        }
    }

    void KFComponentEx::StartSaveEntityTimer( KFEntity* kfentity, KFData* kfdata )
    {
        // 不需要保存
        if ( !kfdata->HaveMask( KFDataDefine::Mask_Save ) ||
                !kfentity->HaveMask( KFDataDefine::Mask_Save ) )
        {
            return;
        }

        // 正在保存中
        if ( static_cast< KFEntityEx* >( kfentity )->_is_in_save )
        {
            return;
        }

        // 启动定时器
        static_cast< KFEntityEx* >( kfentity )->_is_in_save = true;
        __DELAY_TIMER_1__( kfentity->GetKeyID(), kfentity->_data_setting->_delay_save_time, &KFComponentEx::OnTimerSaveEntity );
    }

    __KF_TIMER_FUNCTION__( KFComponentEx::OnTimerSaveEntity )
    {
        auto kfentity = FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        static_cast< KFEntityEx* >( kfentity )->_is_in_save = false;
        SaveEntity( kfentity, KFSaveEnum::UpdateSave, __FUNC_LINE__ );
    }

    void KFComponentEx::DeleteSaveEntity( KFEntity* kfentity )
    {
        switch ( kfentity->_data_setting->_delete_type )
        {
        case KFDataDefine::Data_Delete_Save:
            SaveEntity( kfentity, KFSaveEnum::OfflineSave, __FUNC_LINE__ );
            break;
        case KFDataDefine::Data_Delete_Remove:
            DeleteEntity( kfentity );
            break;
        default:
            break;
        }

        // 删除定时器
        __UN_TIMER_1__( kfentity->GetKeyID() );
    }

    void KFComponentEx::SaveEntity( KFEntity* kfentity, uint32 flag, const char* function, uint32 line )
    {
        if ( _entity_save_function != nullptr )
        {
            _entity_save_function( kfentity, flag );
        }
    }

    void KFComponentEx::DeleteEntity( KFEntity* kfentity )
    {
        if ( _entity_delete_function != nullptr )
        {
            _entity_delete_function( kfentity );
        }
    }

    void KFComponentEx::CallLogElementFunction( KFEntity* kfentity, const std::string& modulename, uint32 operate, KFElement* kfelement )
    {
        auto kffunction = _log_element_function.Find( kfelement->_data_name );
        if ( kffunction == nullptr )
        {
            return;
        }

        kffunction->_function( kfentity, modulename, operate, kfelement );
    }
}