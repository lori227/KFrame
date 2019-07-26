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
                auto kfentity = static_cast< KFEntityEx* >( iter.second );
                _entity_after_run_function( kfentity );
            }
        }

        if ( !_sync_entitys.empty() )
        {
            for ( auto entity : _sync_entitys )
            {
                // 同步到客户端
                auto kfentity = static_cast< KFEntityEx* >( entity );
                kfentity->SyncEntityToClient();
            }
            _sync_entitys.clear();
        }
    }

    VectorString& KFComponentEx::GetDataList( const std::string& dataname )
    {
        static VectorString _result;
        _result.clear();

        auto classsetting = _data_setting->_class_setting;
        auto kfdatasetting = classsetting->FindSetting( dataname );
        if ( kfdatasetting == nullptr || kfdatasetting->_contain_class.empty() )
        {
            return _result;
        }

        for ( auto& iter : classsetting->_static_data._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_contain_class == kfdatasetting->_contain_class )
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
        if ( kfentity == nullptr )
        {
            kfentity = __KF_NEW__( KFEntityEx );
            static_cast< KFEntityEx* >( kfentity )->InitData( this );
            AddEntity( key, kfentity );
        }

        return kfentity;
    }

    KFEntity* KFComponentEx::CreateEntity( uint64 key, const KFMsg::PBObject* proto )
    {
        auto kfentity = CreateEntity( key );
        auto ok = KFKernelModule::Instance()->ParseFromProto( kfentity->GetData(), proto );
        if ( ok )
        {
            kfentity->SetKeyID( key );
            InitEntity( kfentity );
        }
        else
        {
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
        UnInitEntity( kfentity );
        DeleteSaveEntity( kfentity );

        _sync_entitys.erase( kfentity );
        return _entitys.Remove( kfentity->GetKeyID() );
    }

    void KFComponentEx::AddSyncEntity( KFEntity* entity )
    {
        _sync_entitys.insert( entity );
    }

    const std::string& KFComponentEx::GetBindDataName( const std::string& dataname )
    {
        if ( dataname.empty() )
        {
            return _data_setting->_class_setting->_id;
        }

        auto datasetting = _data_setting->_class_setting->FindSetting( dataname );
        if ( datasetting == nullptr || datasetting->_class_setting == nullptr )
        {
            return dataname;
        }

        return datasetting->_class_setting->_id;
    }

    const std::string& KFComponentEx::GetBindDataName( const KFDataSetting* datasetting )
    {
        if ( datasetting->_class_setting == nullptr )
        {
            return datasetting->_name;
        }

        return datasetting->_class_setting->_id;
    }

    void KFComponentEx::SetLogicValue( const std::string& dataname, uint32 value )
    {
        auto kfsetting = FindDataSetting( dataname );
        _kf_option->AddOption( kfsetting->_str_logic_value, value );
    }

    void KFComponentEx::SetLogicValue( const std::string& parentname, const std::string& dataname, uint32 value )
    {
        auto kfsetting = FindDataSetting( parentname, dataname );
        _kf_option->AddOption( kfsetting->_str_logic_value, value );
    }

    void KFComponentEx::SetMaxValue( const std::string& dataname, uint32 value )
    {
        auto kfsetting = FindDataSetting( dataname );
        _kf_option->AddOption( kfsetting->_str_max_value, value );
    }

    void KFComponentEx::SetMaxValue( const std::string& parentname, const std::string& dataname, uint32 value )
    {
        auto kfsetting = FindDataSetting( parentname, dataname );
        _kf_option->AddOption( kfsetting->_str_max_value, value );
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindCheckAddElementFunction( const std::string& dataname, KFCheckAddElementFunction& function )
    {
        auto& bindname = GetBindDataName( dataname );
        auto kffunction = _check_add_element_function.Create( bindname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterCheckAddElementFunction( const std::string& dataname )
    {
        auto& bindname = GetBindDataName( dataname );
        _check_add_element_function.Remove( bindname );
    }

    void KFComponentEx::BindAddElementFunction( const std::string& dataname, KFAddElementFunction& function )
    {
        auto& bindname = GetBindDataName( dataname );
        auto kffunction = _add_element_function.Create( bindname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterAddElementFunction( const std::string& dataname )
    {
        auto& bindname = GetBindDataName( dataname );
        _add_element_function.Remove( bindname );
    }

    void KFComponentEx::BindCheckRemoveElementFunction( const std::string& dataname, KFCheckRemoveElementFunction& function )
    {
        auto& bindname = GetBindDataName( dataname );
        auto kffunction = _check_remove_element_function.Create( bindname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterCheckRemoveElementFunction( const std::string& dataname )
    {
        auto bindname = GetBindDataName( dataname );
        _check_remove_element_function.Remove( bindname );
    }

    void KFComponentEx::BindRemoveElementFunction( const std::string& dataname, KFRemoveElementFunction& function )
    {
        auto& bindname = GetBindDataName( dataname );
        auto kffunction = _remove_element_function.Create( bindname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterRemoveElementFunction( const std::string& dataname )
    {
        auto& bindname = GetBindDataName( dataname );
        _remove_element_function.Remove( bindname );
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

    void KFComponentEx::BindAddDataFunction( const std::string& module, const std::string& dataname, KFAddDataFunction& function )
    {
        auto& bindname = GetBindDataName( dataname );
        auto kfdatafunction = _add_data_function.Create( bindname );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindAddDataFunction( const std::string& module, const std::string& dataname )
    {
        auto& bindname = GetBindDataName( dataname );
        auto kfdatafunction = _add_data_function.Find( bindname );
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

    void KFComponentEx::BindRemoveDataFunction( const std::string& module, const std::string& dataname, KFRemoveDataFunction& function )
    {
        auto& bindname = GetBindDataName( dataname );
        auto kfdatafunction = _remove_data_function.Create( bindname );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindRemoveDataFunction( const std::string& module, const std::string& dataname )
    {
        auto bindname = GetBindDataName( dataname );
        auto kfdatafunction = _remove_data_function.Find( bindname );
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

    void KFComponentEx::BindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname, KFUpdateDataFunction& function )
    {
        auto& bindname = GetBindDataName( parentname );
        auto datakey = DataKeyType( bindname, dataname );
        auto kfdatafunction = _update_data_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname )
    {
        auto& bindname = GetBindDataName( parentname );
        auto datakey = DataKeyType( bindname, dataname );
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
        auto& bindname = GetBindDataName( parentname );
        auto datakey = DataKeyType( bindname, dataname );
        auto kfdatafunction = _update_string_function.Create( datakey );
        kfdatafunction->AddFunction( module, function );
    }

    void KFComponentEx::UnBindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname )
    {
        auto& bindname = GetBindDataName( parentname );
        auto datakey = DataKeyType( bindname, dataname );
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
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::UpdateDataCallBack( KFEntity* kfentity, uint64 key, KFData* kfdata, uint64 index, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        // 更新同步
        kfentity->SyncUpdateData( kfdata, index );

        {
            // 模块回调
            for ( auto& iter : _update_data_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, key, kfdata, operate, value, oldvalue, newvalue );
            }
        }

        {
            // 注册的函数
            auto bindname = GetBindDataName( kfdata->GetParent()->_data_setting );
            auto findkey = DataKeyType( bindname, kfdata->_data_setting->_name );
            auto kfdatafunction = _update_data_function.Find( findkey );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, key, kfdata, operate, value, oldvalue, newvalue );
            }
        }
    }

    void KFComponentEx::UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, const std::string& value )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        kfentity->SyncUpdateData( kfdata, kfentity->GetKeyID() );

        {
            // 模块回调
            for ( auto& iter : _update_string_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, kfdata, value );
            }
        }

        {
            // 注册的函数
            auto bindname = GetBindDataName( kfdata->GetParent()->_data_setting );
            auto findkey = DataKeyType( bindname, kfdata->_data_setting->_name );
            auto kfdatafunction = _update_string_function.Find( findkey );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, kfdata, value );
            }
        }
    }

    void KFComponentEx::AddDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        kfentity->SyncAddData( kfdata, key );

        {
            // 模块回调
            for ( auto& iter : _add_data_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, kfparent, key, kfdata );
            }

        }

        {
            // 注册的函数
            auto bindname = GetBindDataName( kfdata->_data_setting );
            auto kfdatafunction = _add_data_function.Find( bindname );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
            }
        }
    }

    void KFComponentEx::RemoveDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfdata );

        // 同步客户端
        kfentity->SyncRemoveData( kfdata, key );

        {
            // 模块回调
            for ( auto& iter : _remove_data_module._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( kfentity, kfparent, key, kfdata );
            }
        }

        {
            // 注册的函数
            auto bindname = GetBindDataName( kfdata->_data_setting );
            auto kfdatafunction = _remove_data_function.Find( bindname );
            if ( kfdatafunction != nullptr )
            {
                kfdatafunction->CallFunction( kfentity, kfparent, key, kfdata );
            }
        }
    }

    void KFComponentEx::MoveRemoveDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfparent );

        kfentity->SyncRemoveData( kfdata, key );
    }

    void KFComponentEx::MoveAddDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata )
    {
        // 开启保存数据库定时器
        StartSaveEntityTimer( kfentity, kfparent );

        kfentity->SyncAddData( kfdata, key );
    }

    void KFComponentEx::StartSaveEntityTimer( KFEntity* kfentity, KFData* kfdata )
    {
        // 不需要保存
        auto kfobject = kfentity->GetData();
        if ( !kfdata->HaveMask( KFDataDefine::Mask_Save ) ||
                !kfobject->HaveMask( KFDataDefine::Mask_Save ) )
        {
            return;
        }

        // 启动定时器
        if ( !kfentity->IsNeedToSave() )
        {
            kfentity->SetNeetToSave( true );
            __DELAY_TIMER_1__( kfentity->GetKeyID(), kfobject->_data_setting->_delay_save_time, &KFComponentEx::OnTimerSaveEntity );
        }
    }

    __KF_TIMER_FUNCTION__( KFComponentEx::OnTimerSaveEntity )
    {
        auto kfentity = FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        SaveEntity( kfentity, KFSaveEnum::UpdateSave, __FUNC_LINE__ );
    }

    void KFComponentEx::DeleteSaveEntity( KFEntity* kfentity )
    {
        auto kfobject = kfentity->GetData();
        switch ( kfobject->_data_setting->_delete_type )
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
}