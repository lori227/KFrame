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

    void KFComponentEx::InitEntity( EntityPtr kfentity )
    {
        // 初始化
        _entity_initialize_function.Call( kfentity );
    }

    void KFComponentEx::UnInitEntity( EntityPtr kfentity )
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

    StringVector& KFComponentEx::GetDataList( const std::string& data_name )
    {
        static StringVector _result;
        _result.clear();

        auto classsetting = _data_setting->_class_setting;
        for ( auto& iter : classsetting->_static_data._objects )
        {
            auto setting = iter.second;
            if ( setting->_logic_name == data_name )
            {
                _result.push_back( setting->_name );
            }
        }

        return _result;
    }

    // 获得类配置
    const KFClassSetting* KFComponentEx::FindClassSetting()
    {
        return _data_setting->_class_setting;
    }

    const KFClassSetting* KFComponentEx::FindClassSetting( const std::string& data_name )
    {
        auto setting = _data_setting->_class_setting->FindSetting( data_name );
        if ( setting == nullptr )
        {
            return nullptr;
        }

        return setting->_class_setting;
    }

    const KFDataSetting* KFComponentEx::FindDataSetting()
    {
        return _data_setting;
    }

    const KFDataSetting* KFComponentEx::FindDataSetting( const std::string& data_name )
    {
        return _data_setting->_class_setting->FindSetting( data_name );
    }

    const KFDataSetting* KFComponentEx::FindDataSetting( const std::string& parent_name, const std::string& data_name )
    {
        auto setting = _data_setting->_class_setting->FindSetting( parent_name );
        if ( setting == nullptr || setting->_class_setting == nullptr )
        {
            return nullptr;
        }

        return setting->_class_setting->FindSetting( data_name );
    }
    /////////////////////////////////////////////////////////////////////////////////////
    EntityPtr KFComponentEx::FirstEntity()
    {
        return _entitys.First();
    }

    EntityPtr KFComponentEx::NextEntity()
    {
        return _entitys.Next();
    }

    EntityPtr KFComponentEx::CreateEntity( uint64 key )
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

    EntityPtr KFComponentEx::CreateEntity( uint64 key, const KFMsg::PBObject* proto )
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

    void KFComponentEx::AddEntity( uint64 key, EntityPtr kfentity )
    {
        kfentity->SetKeyID( key );
        _entitys.Insert( key, kfentity );
    }

    EntityPtr KFComponentEx::FindEntity( uint64 key )
    {
        return _entitys.Find( key );
    }

    EntityPtr KFComponentEx::FindEntity( uint64 key, const char* function, uint32 line )
    {
        auto kfentity = _entitys.Find( key );
        if ( kfentity == nullptr )
        {
            __LOG_WARN_FUNCTION__( function, line, "can't find entity[{}:{}]", _component_name, key );
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

    bool KFComponentEx::RemoveEntity( EntityPtr kfentity )
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

    void KFComponentEx::AddSyncEntity( EntityPtr entity )
    {
        _sync_entitys.insert( entity );
    }

    void KFComponentEx::SetMaxValue( const std::string& data_name, uint32 value )
    {
        auto setting = FindDataSetting( data_name );
        const_cast< KFDataSetting* >( setting )->_int_max_value = value;
    }

    void KFComponentEx::SetMaxValue( const std::string& parent_name, const std::string& data_name, uint32 value )
    {
        auto setting = FindDataSetting( parent_name, data_name );
        const_cast<KFDataSetting*>( setting )->_int_max_value = value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindCheckAddElementFunction( const std::string& data_name, KFCheckAddElementFunction& function )
    {
        auto kffunction = _check_add_element_function.Create( data_name );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterCheckAddElementFunction( const std::string& data_name )
    {
        _check_add_element_function.Remove( data_name );
    }

    void KFComponentEx::BindAddElementFunction( const std::string& data_name, KFAddElementFunction& function )
    {
        auto kffunction = _add_element_function.Create( data_name );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterAddElementFunction( const std::string& data_name )
    {
        _add_element_function.Remove( data_name );
    }

    void KFComponentEx::BindCheckRemoveElementFunction( const std::string& data_name, KFCheckRemoveElementFunction& function )
    {
        auto kffunction = _check_remove_element_function.Create( data_name );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterCheckRemoveElementFunction( const std::string& data_name )
    {
        _check_remove_element_function.Remove( data_name );
    }

    void KFComponentEx::BindRemoveElementFunction( const std::string& data_name, KFRemoveElementFunction& function )
    {
        auto kffunction = _remove_element_function.Create( data_name );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterRemoveElementFunction( const std::string& data_name )
    {
        _remove_element_function.Remove( data_name );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindAddDataLogic( KFModule* module, const std::string& data_name, KFAddDataFunction& function )
    {
        auto kffunction = _add_logic_function.Create( data_name );
        kffunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataLogic( KFModule* module, const std::string& data_name )
    {
        auto kfdatafunction = _add_logic_function.Find( data_name );
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

    void KFComponentEx::BindAddDataFunction( KFModule* module, const std::string& data_name, uint64 key, KFAddDataFunction& function )
    {
        auto datakey = RecordKeyType( data_name, key );
        auto kfdatafunction = _add_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataFunction( KFModule* module, const std::string& data_name, uint64 key )
    {
        auto datakey = RecordKeyType( data_name, key );
        auto kfdatafunction = _add_data_function.Find( datakey );
        if ( kfdatafunction != nullptr )
        {
            kfdatafunction->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindRemoveDataLogic( KFModule* module, const std::string& data_name, KFRemoveDataFunction& function )
    {
        auto kffunction = _remove_logic_function.Create( data_name );
        kffunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataLogic( KFModule* module, const std::string& data_name )
    {
        auto kfdatafunction = _remove_logic_function.Find( data_name );
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

    void KFComponentEx::BindRemoveDataFunction( KFModule* module, const std::string& data_name, uint64 key, KFRemoveDataFunction& function )
    {
        auto datakey = RecordKeyType( data_name, key );
        auto kfdatafunction = _remove_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataFunction( KFModule* module, const std::string& data_name, uint64 key )
    {
        auto datakey = RecordKeyType( data_name, key );
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

    const DataKeyType& KFComponentEx::FormatDataKey( const std::string& parent_name, const std::string& data_name )
    {
        static DataKeyType _data_key;
        if ( parent_name.empty() )
        {
            _data_key = DataKeyType( _component_name, data_name );
        }
        else
        {
            _data_key = DataKeyType( parent_name, data_name );
        }

        return _data_key;
    }

    const DataKeyType& KFComponentEx::FormatDataKey( DataPtr kfdata )
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

    void KFComponentEx::BindUpdateDataFunction( KFModule* module, const std::string& parent_name, const std::string& data_name, KFUpdateDataFunction& function )
    {
        auto& datakey = FormatDataKey( parent_name, data_name );
        auto kfdatafunction = _update_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateDataFunction( KFModule* module, const std::string& parent_name, const std::string& data_name )
    {
        auto& datakey = FormatDataKey( parent_name, data_name );
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

    void KFComponentEx::BindUpdateStringFunction( KFModule* module, const std::string& parent_name, const std::string& data_name, KFUpdateStringFunction& function )
    {
        auto& datakey = FormatDataKey( parent_name, data_name );
        auto kfdatafunction = _update_string_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateStringFunction( KFModule* module, const std::string& parent_name, const std::string& data_name )
    {
        auto& datakey = FormatDataKey( parent_name, data_name );
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
    void KFComponentEx::UpdateDataCallBack( EntityPtr kfentity, uint64 key, DataPtr kfdata, uint64 index, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        // 更新同步
        kfentity->SyncUpdateDataToClient( kfdata, index );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( kfdata->HaveMask( KFDataDefine::DataMaskUpdateCall ) &&
                kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskUpdateCall ) )
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

    void KFComponentEx::UpdateDataCallBack( EntityPtr kfentity, DataPtr kfdata, const std::string& oldvalue, const std::string& newvalue, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        // 更新同步
        kfentity->SyncUpdateDataToClient( kfdata, kfentity->GetKeyID() );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( kfdata->HaveMask( KFDataDefine::DataMaskUpdateCall ) &&
                kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskUpdateCall ) )
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

    void KFComponentEx::AddDataCallBack( EntityPtr kfentity, DataPtr kfparent, uint64 key, DataPtr kfdata, bool callback )
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
            kfentity->SyncAddDataToClient( kfdata, key );

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
            kfentity->SyncAddDataToClient( kfdata, key );
        }
    }

    void KFComponentEx::RemoveDataCallBack( EntityPtr kfentity, DataPtr kfparent, uint64 key, DataPtr kfdata, bool callback )
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
        kfentity->SyncRemoveDataToClient( kfdata, key );
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

    void KFComponentEx::StartSaveEntityTimer( EntityPtr kfentity, DataPtr kfdata )
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

    void KFComponentEx::DeleteSaveEntity( EntityPtr kfentity )
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

    void KFComponentEx::SaveEntity( EntityPtr kfentity, uint32 flag, const char* function, uint32 line )
    {
        _entity_save_function.Call( kfentity, flag );
    }

    void KFComponentEx::DeleteEntity( EntityPtr kfentity )
    {
        _entity_delete_function.Call( kfentity );
    }

    void KFComponentEx::CallLogElementFunction( EntityPtr kfentity, const KFElementResult* kfresult )
    {
        auto kffunction = _log_element_function.Find( kfresult->_element->_data_name );
        if ( kffunction == nullptr )
        {
            return;
        }

        kffunction->Call( kfentity, kfresult );
    }

    bool KFComponentEx::CallElementResultFunction( EntityPtr kfentity, const KFElementResult* kfresult )
    {
        auto kffunction = _element_result_function.Find( kfresult->_module_name );
        if ( kffunction == nullptr )
        {
            return true;
        }

        return kffunction->CallEx<bool>( kfentity, kfresult );
    }
}