#include "KFComponentEx.hpp"
#include "KFEntityEx.hpp"
#include "KFKernelModule.hpp"
#include "KFKernelConfig.hpp"
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

        _entity_data_mask = 0;
        _entity_delay_save_time = 0;
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

    /////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::SetEntityDataMask( uint32 mask, uint32 savedelaytime )
    {
        _entity_data_mask = mask;
        _entity_delay_save_time = savedelaytime;
    }

    void KFComponentEx::SetName( const std::string& name )
    {
        _component_name = name;
    }

    const std::string& KFComponentEx::GetName()
    {
        return _component_name;
    }

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
            static_cast< KFEntityEx* >( kfentity )->InitData( this, _component_name );
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
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindAddElementFunction( const std::string& dataname, KFAddElementFunction& function )
    {
        auto kffunction = _add_element_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterAddElementFunction( const std::string& dataname )
    {
        _add_element_function.Remove( dataname );
    }

    void KFComponentEx::BindCheckElementFunction( const std::string& dataname, KFCheckElementFunction& function )
    {
        auto kffunction = _check_element_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterCheckElementFunction( const std::string& dataname )
    {
        _check_element_function.Remove( dataname );
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

    void KFComponentEx::BindAddDataFunction( const std::string& module, const std::string& dataname, KFAddDataFunction& function )
    {
        auto kffunction = _add_data_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindAddDataFunction( const std::string& module, const std::string& dataname )
    {
        _add_data_function.Remove( dataname );
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
        auto kffunction = _remove_data_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindRemoveDataFunction( const std::string& module, const std::string& dataname )
    {
        _remove_data_function.Remove( dataname );
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
        auto datakey = DataKeyType( parentname, dataname );
        auto kffunction = _update_data_function.Create( datakey );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname )
    {
        auto datakey = DataKeyType( parentname, dataname );
        _update_data_function.Remove( datakey );
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
        auto datakey = DataKeyType( parentname, dataname );
        auto kffunction = _update_string_function.Create( datakey );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname )
    {
        auto datakey = DataKeyType( parentname, dataname );
        _update_string_function.Remove( datakey );
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
            auto kfparent = kfdata->GetParent();
            auto findkey = DataKeyType( kfparent->GetName(), kfdata->GetName() );
            auto kffunction = _update_data_function.Find( findkey );
            if ( kffunction != nullptr )
            {
                kffunction->_function( kfentity, key, kfdata, operate, value, oldvalue, newvalue );
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
            auto kfparent = kfdata->GetParent();
            auto findkey = DataKeyType( kfparent->GetName(), kfdata->GetName() );
            auto kffunction = _update_string_function.Find( findkey );
            if ( kffunction != nullptr )
            {
                kffunction->_function( kfentity, kfdata, value );
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
            auto kffunction = _add_data_function.Find( kfdata->GetName() );
            if ( kffunction != nullptr )
            {
                kffunction->_function( kfentity, kfparent, key, kfdata );
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
            auto kffunction = _remove_data_function.Find( kfdata->GetName() );
            if ( kffunction != nullptr )
            {
                kffunction->_function( kfentity, kfparent, key, kfdata );
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
        if ( !kfdata->HaveMask( KFDataDefine::Mask_Save ) ||
                !KFUtility::HaveBitMask< uint32 >( _entity_data_mask, KFDataDefine::Data_Save ) )
        {
            return;
        }

        // 启动定时器
        if ( !kfentity->IsNeedToSave() )
        {
            kfentity->SetNeetToSave( true );
            __REGISTER_DELAY_TIMER__( kfentity->GetKeyID(), _entity_delay_save_time, &KFComponentEx::OnTimerSaveEntity );
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
        if ( KFUtility::HaveBitMask< uint32 >( _entity_data_mask, KFDataDefine::Data_Save ) ||
                KFUtility::HaveBitMask< uint32 >( _entity_data_mask, KFDataDefine::Data_Delete_Save ) )
        {
            SaveEntity( kfentity, KFSaveEnum::OfflineSave, __FUNC_LINE__ );
        }
        else if ( KFUtility::HaveBitMask< uint32 >( _entity_data_mask, KFDataDefine::Data_Delete_Remove ) )
        {
            DeleteEntity( kfentity );
        }

        // 删除定时器
        __UNREGISTER_OBJECT_TIMER__( kfentity->GetKeyID() );
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