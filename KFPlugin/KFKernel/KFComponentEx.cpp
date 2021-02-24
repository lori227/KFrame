#include "KFComponentEx.hpp"
#include "KFEntityEx.hpp"
#include "KFKernelModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    KFComponentEx::~KFComponentEx()
    {
        _sync_entity_list.clear();
    }

    void KFComponentEx::OnInitEntity( EntityPtr entity )
    {
        // 初始化
        _entity_init_function.Call( entity );
    }

    void KFComponentEx::OnRemoveEntity( EntityPtr entity )
    {
        entity_remove_function.Call( entity );
    }

    void KFComponentEx::Run()
    {
        if ( _entity_run_function.IsOpen() )
        {
            for ( auto& iter : _entity_list._objects )
            {
                auto entity = iter.second;
                _entity_run_function.Call( entity );
            }
        }
    }

    void KFComponentEx::AfterRun()
    {
        if ( _entity_after_run_function.IsOpen() )
        {
            for ( auto& iter : _entity_list._objects )
            {
                _entity_after_run_function.Call( iter.second );
            }
        }

        if ( !_sync_entity_list.empty() )
        {
            for ( auto entity : _sync_entity_list )
            {
                entity->SyncDataToClient();
            }
            _sync_entity_list.clear();
        }
    }

    StringVector& KFComponentEx::GetDataList( const std::string& data_name )
    {
        static StringVector _result;
        _result.clear();

        auto class_setting = _data_setting->_class_setting;
        for ( auto& iter : class_setting->_static_data._objects )
        {
            auto setting = iter.second;
            if ( setting->_logic_name == data_name )
            {
                _result.push_back( setting->_name );
            }
        }

        return _result;
    }

    std::shared_ptr<const KFDataSetting> KFComponentEx::FindDataSetting()
    {
        return _data_setting;
    }

    std::shared_ptr<const KFDataSetting> KFComponentEx::FindDataSetting( const std::string& data_name )
    {
        return _data_setting->_class_setting->FindSetting( data_name );
    }

    std::shared_ptr<const KFDataSetting> KFComponentEx::FindDataSetting( const std::string& parent_name, const std::string& data_name )
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
        return _entity_list.First();
    }

    EntityPtr KFComponentEx::NextEntity()
    {
        return _entity_list.Next();
    }

    EntityPtr KFComponentEx::CreateEntity( uint64 key )
    {
        auto entity = _entity_list.Find( key );
        if ( entity != nullptr )
        {
            return entity;
        }

        entity = std::static_pointer_cast<KFEntityEx>(KFDataFactory::Instance()->CreateFromDataPool( _data_setting ) );
        if ( entity == nullptr )
        {
            entity = __MAKE_SHARED__( KFEntityEx );
            KFDataFactory::Instance()->InitCreateData( entity, _data_setting );
        }

        std::static_pointer_cast<KFEntityEx>( entity )->InitEntity( shared_from_this() );
        AddEntity( key, entity );
        return entity;
    }

    EntityPtr KFComponentEx::CreateEntity( uint64 key, const KFMsg::PBObject* message )
    {
        auto entity = CreateEntity( key );
        auto ok = _kf_kernel->ParseFromMessage( entity, message );
        if ( ok )
        {
            entity->SetKeyID( key );
            OnInitEntity( entity );
        }
        else
        {
            RemoveEntity( entity );
            entity = nullptr;
            __LOG_ERROR__( "[{}:{}] create error", _name, key );
        }

        return entity;
    }

    void KFComponentEx::AddEntity( uint64 key, EntityPtr entity )
    {
        entity->SetKeyID( key );
        _entity_list.Insert( key, entity );
    }

    EntityPtr KFComponentEx::FindEntity( uint64 key )
    {
        return _entity_list.Find( key );
    }

    EntityPtr KFComponentEx::FindEntity( uint64 key, const char* function, uint32 line )
    {
        auto entity = _entity_list.Find( key );
        if ( entity == nullptr )
        {
            __LOG_WARN_FUNCTION__( function, line, "can't find entity[{}:{}]", _name, key );
        }

        return entity;
    }

    uint32 KFComponentEx::GetEntityCount()
    {
        return _entity_list.Size();
    }

    bool KFComponentEx::RemoveEntity( uint64 key )
    {
        auto entity = FindEntity( key, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return false;
        }

        return RemoveEntity( entity );
    }

    bool KFComponentEx::RemoveEntity( EntityPtr entity )
    {
        // 释放资源, 保存
        if ( entity->IsInited() )
        {
            OnRemoveEntity( entity );
            DeleteSaveEntity( entity );
        }

        _sync_entity_list.erase( entity );
        KFDataFactory::Instance()->DestroyData( entity );
        _entity_list.Remove( entity->GetKeyID() );
        return true;
    }

    void KFComponentEx::AddSyncEntity( EntityPtr entity )
    {
        _sync_entity_list.insert( entity );
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindCheckAddElementFunction( KFModule* module, const std::string& data_name, KFCheckAddElementFunction& function )
    {
        auto function_data = _check_add_element_function.Create( data_name );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterCheckAddElementFunction( const std::string& data_name )
    {
        _check_add_element_function.Remove( data_name );
    }

    void KFComponentEx::BindAddElementFunction( KFModule* module, const std::string& data_name, KFAddElementFunction& function )
    {
        auto function_data = _add_element_function.Create( data_name );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterAddElementFunction( const std::string& data_name )
    {
        _add_element_function.Remove( data_name );
    }

    void KFComponentEx::BindCheckRemoveElementFunction( KFModule* module, const std::string& data_name, KFCheckRemoveElementFunction& function )
    {
        auto function_data = _check_remove_element_function.Create( data_name );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterCheckRemoveElementFunction( const std::string& data_name )
    {
        _check_remove_element_function.Remove( data_name );
    }

    void KFComponentEx::BindRemoveElementFunction( KFModule* module, const std::string& data_name, KFRemoveElementFunction& function )
    {
        auto function_data = _remove_element_function.Create( data_name );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterRemoveElementFunction( const std::string& data_name )
    {
        _remove_element_function.Remove( data_name );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindAddDataLogic( KFModule* module, const std::string& data_name, KFAddDataFunction& function )
    {
        auto data_function = _add_logic_function.Create( data_name );
        data_function->AddFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataLogic( KFModule* module, const std::string& data_name )
    {
        auto data_function = _add_logic_function.Find( data_name );
        if ( data_function != nullptr )
        {
            data_function->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindAddDataModule( KFModule* module, KFAddDataFunction& function )
    {
        auto function_data = _add_data_module.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataModule( KFModule* module )
    {
        _add_data_module.Remove( module );
    }

    void KFComponentEx::BindAddDataFunction( KFModule* module, const std::string& data_name, uint64 key, KFAddDataFunction& function )
    {
        auto data_key = RecordKeyType( data_name, key );
        auto data_function = _add_data_function.Create( data_key );
        data_function->AddFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataFunction( KFModule* module, const std::string& data_name, uint64 key )
    {
        auto data_key = RecordKeyType( data_name, key );
        auto data_function = _add_data_function.Find( data_key );
        if ( data_function != nullptr )
        {
            data_function->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindRemoveDataLogic( KFModule* module, const std::string& data_name, KFRemoveDataFunction& function )
    {
        auto data_function = _remove_logic_function.Create( data_name );
        data_function->AddFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataLogic( KFModule* module, const std::string& data_name )
    {
        auto data_function = _remove_logic_function.Find( data_name );
        if ( data_function != nullptr )
        {
            data_function->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindRemoveDataModule( KFModule* module, KFRemoveDataFunction& function )
    {
        auto function_data = _remove_data_module.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataModule( KFModule* module )
    {
        _remove_data_module.Remove( module );
    }

    void KFComponentEx::BindRemoveDataFunction( KFModule* module, const std::string& data_name, uint64 key, KFRemoveDataFunction& function )
    {
        auto data_key = RecordKeyType( data_name, key );
        auto data_function = _remove_data_function.Create( data_key );
        data_function->AddFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataFunction( KFModule* module, const std::string& data_name, uint64 key )
    {
        auto data_key = RecordKeyType( data_name, key );
        auto data_function = _remove_data_function.Find( data_key );
        if ( data_function != nullptr )
        {
            data_function->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindUpdateDataModule( KFModule* module, KFUpdateDataFunction& function )
    {
        auto function_data = _update_data_module.Create( module );
        function_data->SetFunction( module, function );
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
            _data_key = DataKeyType( _name, data_name );
        }
        else
        {
            _data_key = DataKeyType( parent_name, data_name );
        }

        return _data_key;
    }

    const DataKeyType& KFComponentEx::FormatDataKey( DataPtr data )
    {
        auto parent_data = data->GetParent();
        if ( parent_data->_data_setting->_type == KFDataDefine::DataTypeArray )
        {
            if ( parent_data->GetParent() != nullptr )
            {
                return FormatDataKey( parent_data->GetParent()->_data_setting->_logic_name, parent_data->_data_setting->_logic_name );
            }
        }

        return FormatDataKey( parent_data->_data_setting->_logic_name, parent_data->_data_setting->_logic_name );
    }

    void KFComponentEx::BindUpdateDataFunction( KFModule* module, const std::string& parent_name, const std::string& data_name, KFUpdateDataFunction& function )
    {
        auto& data_key = FormatDataKey( parent_name, data_name );
        auto data_function = _update_data_function.Create( data_key );
        data_function->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateDataFunction( KFModule* module, const std::string& parent_name, const std::string& data_name )
    {
        auto& data_key = FormatDataKey( parent_name, data_name );
        auto data_function = _update_data_function.Find( data_key );
        if ( data_function != nullptr )
        {
            data_function->RemoveFunction( module );
        }
    }

    void KFComponentEx::BindUpdateStringModule( KFModule* module, KFUpdateStringFunction& function )
    {
        auto function_data = _update_string_module.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateStringModule( KFModule* module )
    {
        _update_string_module.Remove( module );
    }

    void KFComponentEx::BindUpdateStringFunction( KFModule* module, const std::string& parent_name, const std::string& data_name, KFUpdateStringFunction& function )
    {
        auto& data_key = FormatDataKey( parent_name, data_name );
        auto data_function = _update_string_function.Create( data_key );
        data_function->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateStringFunction( KFModule* module, const std::string& parent_name, const std::string& data_name )
    {
        auto& data_key = FormatDataKey( parent_name, data_name );
        auto data_function = _update_string_function.Find( data_key );
        if ( data_function != nullptr )
        {
            data_function->RemoveFunction( module );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindEntityInitializeFunction( KFModule* module, KFEntityFunction& function )
    {
        _entity_init_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterEntityInitializeFunction()
    {
        _entity_init_function.Reset();
    }

    void KFComponentEx::BindEntityRemoveFunction( KFModule* module, KFEntityFunction& function )
    {
        entity_remove_function.SetFunction( module, function );
    }

    void KFComponentEx::UnRegisterEntityRemoveFunction()
    {
        entity_remove_function.Reset();
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
        auto function_data = _get_config_value_function.Create( name );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnBindGetConfigValueFunction( const std::string& name )
    {
        _get_config_value_function.Remove( name );
    }

    void KFComponentEx::BindLogElementFunction( KFModule* module, const std::string& name, KFLogElementFunction& function )
    {
        auto function_data = _log_element_function.Create( name );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnBindLogElementFunction( const std::string& name )
    {
        _log_element_function.Remove( name );
    }

    void KFComponentEx::BindElementResultFunction( KFModule* module, const std::string& name, KFElementResultFunction& function )
    {
        auto function_data = _element_result_function.Create( name );
        function_data->SetFunction( module, function );
    }

    void KFComponentEx::UnBindElementResultFunction( const std::string& name )
    {
        _element_result_function.Remove( name );
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::UpdateDataCallBack( EntityPtr entity, uint64 key, DataPtr data, uint64 index, uint32 operate, uint64 value, uint64 old_value, uint64 new_value, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( entity, data );

        // 更新同步
        entity->SyncUpdateDataToClient( data, index );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( data->HaveMask( KFDataDefine::DataMaskUpdateCall ) &&
                data->GetParent()->HaveMask( KFDataDefine::DataMaskUpdateCall ) )
        {
            for ( auto& iter : _update_data_module._objects )
            {
                auto function = iter.second;
                function->Call( entity, key, data, operate, value, old_value, new_value );
            }
        }

        // 注册的函数
        auto& data_key = FormatDataKey( data );
        auto data_function = _update_data_function.Find( data_key );
        if ( data_function != nullptr )
        {
            data_function->CallFunction( entity, key, data, operate, value, old_value, new_value );
        }
    }

    void KFComponentEx::UpdateDataCallBack( EntityPtr entity, DataPtr data, const std::string& old_value, const std::string& new_value, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( entity, data );

        // 更新同步
        entity->SyncUpdateDataToClient( data, entity->GetKeyID() );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( data->HaveMask( KFDataDefine::DataMaskUpdateCall ) &&
                data->GetParent()->HaveMask( KFDataDefine::DataMaskUpdateCall ) )
        {
            for ( auto& iter : _update_string_module._objects )
            {
                auto function = iter.second;
                function->Call( entity, data, old_value, new_value );
            }
        }

        // 注册的函数
        auto& data_key = FormatDataKey( data );
        auto data_function = _update_string_function.Find( data_key );
        if ( data_function != nullptr )
        {
            data_function->CallFunction( entity, data, old_value, new_value );
        }
    }

    void KFComponentEx::AddDataCallBack( EntityPtr entity, DataPtr parent_data, uint64 key, DataPtr data, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( entity, data );
        {
            // 逻辑回调
            auto data_function = _add_logic_function.Find( parent_data->_data_setting->_logic_name );
            if ( data_function != nullptr )
            {
                data_function->CallFunction( entity, parent_data, key, data );
            }
        }

        if ( callback )
        {
            // 注册的函数
            {
                auto data_key = RecordKeyType( data->_data_setting->_logic_name, key );
                auto data_function = _add_data_function.Find( data_key );
                if ( data_function != nullptr )
                {
                    data_function->CallFunction( entity, parent_data, key, data );
                }
            }
            {
                auto data_key = RecordKeyType( data->_data_setting->_logic_name, 0u );
                auto data_function = _add_data_function.Find( data_key );
                if ( data_function != nullptr )
                {
                    data_function->CallFunction( entity, parent_data, key, data );
                }
            }

            // 同步属性
            entity->SyncAddDataToClient( data, key );

            // 模块回调
            if ( data->HaveMask( KFDataDefine::DataMaskAddCall ) )
            {
                for ( auto& iter : _add_data_module._objects )
                {
                    auto function = iter.second;
                    function->Call( entity, parent_data, key, data );
                }
            }
        }
        else
        {
            // 同步属性
            entity->SyncAddDataToClient( data, key );
        }
    }

    void KFComponentEx::RemoveDataCallBack( EntityPtr entity, DataPtr parent_data, uint64 key, DataPtr data, bool callback )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( entity, data );

        // 逻辑回调
        {
            auto data_function = _remove_logic_function.Find( parent_data->_data_setting->_logic_name );
            if ( data_function != nullptr )
            {
                data_function->CallFunction( entity, parent_data, key, data );
            }
        }

        // 同步客户端
        entity->SyncRemoveDataToClient( data, key );
        if ( !callback )
        {
            return;
        }

        // 模块回调
        if ( data->HaveMask( KFDataDefine::DataMaskRemoveCall ) )
        {
            for ( auto& iter : _remove_data_module._objects )
            {
                auto function = iter.second;
                function->Call( entity, parent_data, key, data );
            }
        }

        // 注册的函数
        {
            auto data_key = RecordKeyType( parent_data->_data_setting->_logic_name, key );
            auto data_function = _remove_data_function.Find( data_key );
            if ( data_function != nullptr )
            {
                data_function->CallFunction( entity, parent_data, key, data );
            }
        }
        {
            auto data_key = RecordKeyType( parent_data->_data_setting->_logic_name, 0u );
            auto data_function = _remove_data_function.Find( data_key );
            if ( data_function != nullptr )
            {
                data_function->CallFunction( entity, parent_data, key, data );
            }
        }
    }

    void KFComponentEx::StartSaveEntityTimer( EntityPtr entity, DataPtr data )
    {
        // 不需要保存
        if ( !data->HaveMask( KFDataDefine::DataMaskSave ) ||
                !entity->HaveMask( KFDataDefine::DataMaskSave ) )
        {
            return;
        }

        // 正在保存中
        if ( std::static_pointer_cast<KFEntityEx>( entity )->_is_in_save )
        {
            return;
        }

        // 启动定时器
        std::static_pointer_cast<KFEntityEx>( entity )->_is_in_save = true;
        __DELAY_TIMER_1__( entity->GetKeyID(), entity->_data_setting->_delay_save_time, &KFComponentEx::OnTimerSaveEntity );
    }

    __KF_TIMER_FUNCTION__( KFComponentEx::OnTimerSaveEntity )
    {
        auto entity = FindEntity( object_id );
        if ( entity == nullptr )
        {
            return;
        }

        std::static_pointer_cast<KFEntityEx>( entity )->_is_in_save = false;
        SaveEntity( entity, KFSaveEnum::UpdateSave, __FUNC_LINE__ );
    }

    void KFComponentEx::DeleteSaveEntity( EntityPtr entity )
    {
        switch ( entity->_data_setting->_delete_type )
        {
        case KFDataDefine::DataDeleteSave:
            SaveEntity( entity, KFSaveEnum::OfflineSave, __FUNC_LINE__ );
            break;
        case KFDataDefine::DataDeleteRemove:
            DeleteEntity( entity );
            break;
        default:
            break;
        }

        // 删除定时器
        __UN_TIMER_1__( entity->GetKeyID() );
    }

    void KFComponentEx::SaveEntity( EntityPtr entity, uint32 flag, const char* function, uint32 line )
    {
        _entity_save_function.Call( entity, flag );
    }

    void KFComponentEx::DeleteEntity( EntityPtr entity )
    {
        _entity_delete_function.Call( entity );
    }

    void KFComponentEx::CallLogElementFunction( EntityPtr entity, const KFElementResult* result )
    {
        auto function = _log_element_function.Find( result->_element->_data_name );
        if ( function != nullptr )
        {
            function->Call( entity, result );
        }
    }

    bool KFComponentEx::CallElementResultFunction( EntityPtr entity, const KFElementResult* result )
    {
        auto function = _element_result_function.Find( result->_module_name );
        if ( function == nullptr )
        {
            return true;
        }

        return function->CallEx<bool>( entity, result );
    }
}