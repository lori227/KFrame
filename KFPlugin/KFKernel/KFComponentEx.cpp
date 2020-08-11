#include "KFComponentEx.hpp"
#include "KFEntityEx.hpp"
#include "KFKernelModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    KFComponentEx::~KFComponentEx()
    {
        _sync_entitys.clear();
    }

    void KFComponentEx::InitEntity( KFEntity* kfentity )
    {
        // 初始化
        _entity_initialize_function.Call( kfentity );
    }

    void KFComponentEx::UnInitEntity( KFEntity* kfentity )
    {
        _entity_uninitialize_function.Call( kfentity );
    }

    void KFComponentEx::Run()
    {
        if ( _entity_run_function.IsOpen() )
        {
            for ( auto& iter : _entitys._objects )
            {
                auto kfentity = iter.second;
                _entity_run_function._function( kfentity );
            }
        }
    }

    void KFComponentEx::AfterRun()
    {
        if ( _entity_after_run_function.IsOpen() )
        {
            for ( auto& iter : _entitys._objects )
            {
                _entity_after_run_function._function( iter.second );
            }
        }

        if ( !_sync_entitys.empty() )
        {
            for ( auto entity : _sync_entitys )
            {
                entity->SyncDataToClient();
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
        if ( kfentity == nullptr )
        {
            kfentity = __KF_NEW__( KFEntityEx );
            KFDataFactory::Instance()->InitCreateData( kfentity, _data_setting );
        }

        static_cast< KFEntityEx* >( kfentity )->InitEntity( this );
        AddEntity( key, kfentity );
        return kfentity;
    }

    KFEntity* KFComponentEx::CreateEntity( uint64 key, const KFMsg::PBObject* proto )
    {
        auto kfentity = CreateEntity( key );
        auto ok = _kf_kernel->ParseFromProto( kfentity, proto );
        if ( ok )
        {
            kfentity->SetKeyID( key );
            InitEntity( kfentity );
        }
        else
        {
            RemoveEntity( kfentity );
            kfentity = nullptr;
            __LOG_ERROR__( "[{}:{}] create error", _component_name, key );
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
            __LOG_ERROR_FUNCTION__( function, line, "can't find entity[{}:{}]", _component_name, key );
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
    void KFComponentEx::BindAddDataLogic( KFModule* module, const std::string& dataname, KFAddDataFunction& function )
    {
        auto kffunction = _add_logic_function.Create( dataname );
        kffunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataLogic( KFModule* module, const std::string& dataname )
    {
        auto kfdatafunction = _add_logic_function.Find( dataname );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindAddDataModule( KFModule* module, KFAddDataFunction& function )
    {
        auto kffunction = _add_data_module.Create( module );
        kffunction->SetFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataModule( KFModule* module )
    {
        _add_data_module.Remove( module );
    }

    void KFComponentEx::BindAddDataFunction( KFModule* module, const std::string& dataname, uint64 key, KFAddDataFunction& function )
    {
        auto datakey = RecordKeyType( dataname, key );
        auto kfdatafunction = _add_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataFunction( KFModule* module, const std::string& dataname, uint64 key )
    {
        auto datakey = RecordKeyType( dataname, key );
        auto kfdatafunction = _add_data_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindRemoveDataLogic( KFModule* module, const std::string& dataname, KFRemoveDataFunction& function )
    {
        auto kffunction = _remove_logic_function.Create( dataname );
        kffunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataLogic( KFModule* module, const std::string& dataname )
    {
        auto kfdatafunction = _remove_logic_function.Find( dataname );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindRemoveDataModule( KFModule* module, KFRemoveDataFunction& function )
    {
        auto kffunction = _remove_data_module.Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindRemoveDataModule( KFModule* module )
    {
        _remove_data_module.Remove( module );
    }

    void KFComponentEx::BindRemoveDataFunction( KFModule* module, const std::string& dataname, uint64 key, KFRemoveDataFunction& function )
    {
        auto datakey = RecordKeyType( dataname, key );
        auto kfdatafunction = _remove_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataFunction( KFModule* module, const std::string& dataname, uint64 key )
    {
        auto datakey = RecordKeyType( dataname, key );
        auto kfdatafunction = _remove_data_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindUpdateDataModule( KFModule* module, KFUpdateDataFunction& function )
    {
        auto kffunction = _update_data_module.Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindUpdateDataModule( KFModule* module )
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
        if ( kfparent->_data_setting->_type == KFDataDefine::DataTypeArray )
        {
            if ( kfparent->GetParent() != nullptr )
            {
                return FormatDataKey( kfparent->GetParent()->_data_setting->_logic_name, kfparent->_data_setting->_logic_name );
            }
        }

        return FormatDataKey( kfparent->_data_setting->_logic_name, kfdata->_data_setting->_logic_name );
    }

    void KFComponentEx::BindUpdateDataFunction( KFModule* module, const std::string& parentname, const std::string& dataname, KFUpdateDataFunction& function )
    {
        auto& datakey = FormatDataKey( parentname, dataname );
        auto kfdatafunction = _update_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateDataFunction( KFModule* module, const std::string& parentname, const std::string& dataname )
    {
        auto& datakey = FormatDataKey( parentname, dataname );
        auto kfdatafunction = _update_data_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindUpdateStringModule( KFModule* module, KFUpdateStringFunction& function )
    {
        auto kffunction = _update_string_module.Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindUpdateStringModule( KFModule* module )
    {
        _update_string_module.Remove( module );
    }

    void KFComponentEx::BindUpdateStringFunction( KFModule* module, const std::string& parentname, const std::string& dataname, KFUpdateStringFunction& function )
    {
        auto& datakey = FormatDataKey( parentname, dataname );
        auto kfdatafunction = _update_string_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateStringFunction( KFModule* module, const std::string& parentname, const std::string& dataname )
    {
        auto& datakey = FormatDataKey( parentname, dataname );
        auto kfdatafunction = _update_string_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindEntityInitializeFunction( KFModule* module, KFEntityFunction& function )
    {
        _entity_initialize_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterEntityInitializeFunction()
    {
        _entity_initialize_function.Reset();
    }

    void KFComponentEx::BindEntityUninitializeFunction( KFModule* module, KFEntityFunction& function )
    {
        _entity_uninitialize_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterEntityUninitializeFunction()
    {
        _entity_uninitialize_function.Reset();
    }

    void KFComponentEx::BindEntityRunFunction( KFModule* module, KFEntityFunction& function )
    {
        _entity_run_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterEntityRunFunction()
    {
        _entity_run_function.Reset();
    }

    void KFComponentEx::BindEntityAfterRunFunction( KFModule* module, KFEntityFunction& function )
    {
        _entity_after_run_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterEntityAfterRunFunction()
    {
        _entity_after_run_function.Reset();
    }

    void KFComponentEx::BindEntitySaveFunction( KFModule* module, KFSaveEntityFunction& function )
    {
        _entity_save_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterEntitySaveFunction()
    {
        _entity_save_function.Reset();
    }

    void KFComponentEx::BindEntityDeleteFunction( KFModule* module, KFEntityFunction& function )
    {
        _entity_delete_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterEntityDeleteFunction()
    {
        _entity_delete_function.Reset();
    }

    void KFComponentEx::BindSyncAddFunction( KFModule* module, KFSyncFunction& function )
    {
        _entity_sync_add_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterSyncAddFunction()
    {
        _entity_sync_add_function.Reset();
    }

    void KFComponentEx::BindSyncRemoveFunction( KFModule* module, KFSyncFunction& function )
    {
        _entity_sync_remove_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterSyncRemoveFunction()
    {
        _entity_sync_remove_function.Reset();
    }

    void KFComponentEx::BindSyncUpdateFunction( KFModule* module, KFSyncFunction& function )
    {
        _entity_sync_update_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterSyncUpdateFunction()
    {
        _entity_sync_update_function.Reset();
    }

    void KFComponentEx::BindShowElementFunction( KFModule* module, KFShowElementFunction& function )
    {
        _show_element_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterShowElementFunction()
    {
        _show_element_function.Reset();
    }

    void KFComponentEx::BindGetConfigValueFunction( KFModule* module, const std::string& name, KFGetConfigValueFunction& function )
    {
        auto kffunction = _get_config_value_function.Create( name );
        kffunction->SetFunction( module, function );
    }

    void KFComponentEx::UnBindGetConfigValueFunction( const std::string& name )
    {
        _get_config_value_function.Remove( name );
    }

    void KFComponentEx::BindLogElementFunction( KFModule* module, const std::string& name, KFLogElementFunction& function )
    {
        auto kffunction = _log_element_function.Create( name );
        kffunction->SetFunction( module, function );
    }

    void KFComponentEx::UnBindLogElementFunction( const std::string& name )
    {
        _log_element_function.Remove( name );
    }

    void KFComponentEx::BindElementResultFunction( KFModule* module, const std::string& name, KFElementResultFunction& function )
    {
        auto kffunction = _element_result_function.Create( name );
        kffunction->SetFunction( module, function );
    }

    void KFComponentEx::UnBindElementResultFunction( const std::string& name )
    {
        _element_result_function.Remove( name );
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
        if ( kfdata->HaveMask( KFDataDefine::DataMaskUpdataCall ) &&
                kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskUpdataCall ) )
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

    void KFComponentEx::UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, const std::string& oldvalue, const std::string& newvalue, bool callback )
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
        if ( kfdata->HaveMask( KFDataDefine::DataMaskUpdataCall ) &&
                kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskUpdataCall ) )
        {
            for ( auto& iter : _update_string_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, kfdata, oldvalue, newvalue );
            }
        }

        // 注册的函数
        auto& findkey = FormatDataKey( kfdata );
        auto kfdatafunction = _update_string_function.Find( findkey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->CallFunction( kfentity, kfdata, oldvalue, newvalue );
        }
    }

    void KFComponentEx::AddDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );
        {
            // 逻辑回调
            auto kfdatafunction = _add_logic_function.Find( kfparent->_data_setting->_logic_name );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
            }
        }

        if ( callback )
        {
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

            // 同步属性
            kfentity->SyncAddData( kfdata, key );

            // 模块回调
            if ( kfdata->HaveMask( KFDataDefine::DataMaskAddCall ) )
            {
                for ( auto& iter : _add_data_module._objects )
                {
                    auto kffunction = iter.second;
                    kffunction->_function( kfentity, kfparent, key, kfdata );
                }
            }
        }
        else
        {
            // 同步属性
            kfentity->SyncAddData( kfdata, key );
        }
    }

    void KFComponentEx::RemoveDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        // 逻辑回调
        {
            auto kfdatafunction = _remove_logic_function.Find( kfparent->_data_setting->_logic_name );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
            }
        }

        // 同步客户端
        kfentity->SyncRemoveData( kfdata, key );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( kfdata->HaveMask( KFDataDefine::DataMaskRemoveCall ) )
        {
            for ( auto& iter : _remove_data_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, kfparent, key, kfdata );
            }
        }

        // 注册的函数
        {
            auto findkey = RecordKeyType( kfparent->_data_setting->_logic_name, key );
            auto kfdatafunction = _remove_data_function.Find( findkey );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
            }
        }
        {
            auto findkey = RecordKeyType( kfparent->_data_setting->_logic_name, 0u );
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
        if ( !kfdata->HaveMask( KFDataDefine::DataMaskSave ) ||
                !kfentity->HaveMask( KFDataDefine::DataMaskSave ) )
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
        auto kfentity = FindEntity( objectid );
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
        case KFDataDefine::DataDeleteSave:
            SaveEntity( kfentity, KFSaveEnum::OfflineSave, __FUNC_LINE__ );
            break;
        case KFDataDefine::DataDeleteRemove:
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
        _entity_save_function.Call( kfentity, flag );
    }

    void KFComponentEx::DeleteEntity( KFEntity* kfentity )
    {
        _entity_delete_function.Call( kfentity );
    }

    void KFComponentEx::CallLogElementFunction( KFEntity* kfentity, const KFElementResult* kfresult )
    {
        auto kffunction = _log_element_function.Find( kfresult->_element->_data_name );
        if ( kffunction == nullptr )
        {
            return;
        }

        kffunction->Call( kfentity, kfresult );
    }

    bool KFComponentEx::CallElementResultFunction( KFEntity* kfentity, const KFElementResult* kfresult )
    {
        auto kffunction = _element_result_function.Find( kfresult->_module_name );
        if ( kffunction == nullptr )
        {
            return true;
        }

        return kffunction->CallEx<bool>( kfentity, kfresult );
    }
}