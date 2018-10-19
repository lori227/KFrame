#include "KFComponentEx.h"
#include "KFEntityEx.h"
#include "KFKernelModule.h"
#include "KFKernelConfig.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    KFComponentEx::KFComponentEx()
    {
        _kf_kernel_module = nullptr;

        _entity_initialize_function = nullptr;
        _entity_uninitialize_function = nullptr;
        _entity_run_function = nullptr;
        _entity_after_run_function = nullptr;
        _entity_save_function = nullptr;
        _entity_delete_function = nullptr;

        _entity_sync_add_function = nullptr;
        _entity_sync_update_function = nullptr;
        _entity_sync_remove_function = nullptr;
        _show_reward_function = nullptr;

        _entity_data_mask = 0;
    }

    KFComponentEx::~KFComponentEx()
    {

    }

    void KFComponentEx::InitEntity( KFEntity* kfentity )
    {
        // 初始化
        if ( _entity_initialize_function != nullptr )
        {
            _entity_initialize_function( kfentity );
        }

        kfentity->SetInited();
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
        if ( _entity_run_function == nullptr )
        {
            return;
        }

        for ( auto& iter : _entitys._objects )
        {
            auto kfentity = iter.second;
            _entity_run_function( kfentity );
        }
    }

    void KFComponentEx::AfterRun()
    {
        for ( auto& iter : _entitys._objects )
        {
            auto kfentity = static_cast< KFEntityEx* >( iter.second );

            if ( _entity_after_run_function != nullptr )
            {
                _entity_after_run_function( kfentity );
            }

            // 同步到客户端
            kfentity->SyncEntityToClient();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::SetKernelModule( KFKernelModule* module )
    {
        _kf_kernel_module = module;
    }

    void KFComponentEx::SetEntityDataMask( uint32 mask )
    {
        _entity_data_mask = mask;
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
        auto kfentity = __KF_CREATE__( KFEntityEx );
        kfentity->InitData( this, _component_name );

        AddEntity( key, kfentity );
        return kfentity;
    }

    KFEntity* KFComponentEx::CreateEntity( uint64 key, const KFMsg::PBObject* proto )
    {
        auto kfentity = CreateEntity( key );
        _kf_kernel_module->ParseFromProto( kfentity->GetData(), proto );
        kfentity->SetKeyID( key );

        InitEntity( kfentity );
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
            __LOG_INFO_FUNCTION__( function, line, "can't find entity[{}:{}]!", _component_name, key );
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

        auto key = kfentity->GetKeyID();
        return _entitys.Remove( key );
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::BindAddAgentFunction( const std::string& dataname, KFAddAgentFunction& function )
    {
        auto kffunction = _add_agent_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterAddAgentFunction( const std::string& dataname )
    {
        _add_agent_function.Remove( dataname );
    }

    void KFComponentEx::BindCheckAgentFunction( const std::string& dataname, KFCheckAgentFunction& function )
    {
        auto kffunction = _check_agent_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterCheckAgentFunction( const std::string& dataname )
    {
        _check_agent_function.Remove( dataname );
    }

    void KFComponentEx::BindRemoveAgentFunction( const std::string& dataname, KFRemoveAgentFunction& function )
    {
        auto kffunction = _remove_agent_function.Create( dataname );
        kffunction->_function = function;
    }

    void KFComponentEx::UnRegisterRemoveAgentFunction( const std::string& dataname )
    {
        _remove_agent_function.Remove( dataname );
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
        auto kfbind = _add_data_function.Create( dataname );
        auto kffunction = kfbind->Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindAddDataFunction( const std::string& module, const std::string& dataname )
    {
        auto kfbind = _add_data_function.Find( dataname );
        if ( kfbind != nullptr )
        {
            kfbind->Remove( module );
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
        auto kfbind = _remove_data_function.Create( dataname );
        auto kffunction = kfbind->Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindRemoveDataFunction( const std::string& module, const std::string& dataname )
    {
        auto kfbind = _remove_data_function.Find( dataname );
        if ( kfbind != nullptr )
        {
            kfbind->Remove( module );
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
        auto datakey = DataKeyType( parentname, dataname );
        auto kfbind = _update_data_function.Create( datakey );
        auto kffunction = kfbind->Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindUpdateDataFunction( const std::string& module, const std::string& parentname, const std::string& dataname )
    {
        auto datakey = DataKeyType( parentname, dataname );
        auto kfbind = _update_data_function.Find( datakey );
        if ( kfbind != nullptr )
        {
            kfbind->Remove( module );
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
        auto datakey = DataKeyType( parentname, dataname );

        auto kfbind = _update_string_function.Create( datakey );
        auto kffunction = kfbind->Create( module );
        kffunction->_function = function;
    }

    void KFComponentEx::UnBindUpdateStringFunction( const std::string& module, const std::string& parentname, const std::string& dataname )
    {
        auto datakey = DataKeyType( parentname, dataname );
        auto kfbind = _update_string_function.Find( datakey );
        if ( kfbind != nullptr )
        {
            kfbind->Remove( module );
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

    void KFComponentEx::BindEntitySaveFunction( KFEntityFunction& function )
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

    void KFComponentEx::BindShowRewardFunction( KFShowRewardFunction& function )
    {
        _show_reward_function = function;
    }

    void KFComponentEx::UnRegisterShowRewardFunction()
    {
        _show_reward_function = nullptr;
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    void KFComponentEx::UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, uint64 key, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue )
    {
        if ( kfentity->IsInited() )
        {
            static_cast< KFEntityEx* >( kfentity )->SyncUpdateData( kfdata, key );
        }

        // 开启保存数据库定时器
        if ( kfdata->GetDataSetting()->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            StartSaveEntityTimer( kfentity );
        }

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
            auto kfbind = _update_data_function.Find( findkey );
            if ( kfbind != nullptr )
            {
                for ( auto& iter : kfbind->_objects )
                {
                    auto kffunction = iter.second;
                    kffunction->_function( kfentity, key, kfdata, operate, value, oldvalue, newvalue );
                }
            }
        }
    }

    void KFComponentEx::UpdateDataCallBack( KFEntity* kfentity, KFData* kfdata, const std::string& value )
    {
        if ( kfentity->IsInited() )
        {
            static_cast< KFEntityEx* >( kfentity )->SyncUpdateData( kfdata, kfentity->GetKeyID() );
        }

        // 开启保存数据库定时器
        if ( kfdata->GetDataSetting()->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            StartSaveEntityTimer( kfentity );
        }

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
            auto kfbind = _update_string_function.Find( findkey );
            if ( kfbind != nullptr )
            {
                for ( auto& iter : kfbind->_objects )
                {
                    auto kffunction = iter.second;
                    kffunction->_function( kfentity, kfdata, value );
                }
            }
        }
    }

    void KFComponentEx::AddDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata )
    {
        if ( kfentity->IsInited() )
        {
            static_cast< KFEntityEx* >( kfentity )->SyncAddData( kfdata, key );
        }

        // 开启保存数据库定时器
        if ( kfdata->GetDataSetting()->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            StartSaveEntityTimer( kfentity );
        }

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
            auto kfbind = _add_data_function.Find( kfdata->GetName() );
            if ( kfbind != nullptr )
            {
                for ( auto& iter : kfbind->_objects )
                {
                    auto kffunction = iter.second;
                    kffunction->_function( kfentity, kfparent, key, kfdata );
                }
            }
        }
    }

    void KFComponentEx::RemoveDataCallBack( KFEntity* kfentity, KFData* kfparent, uint64 key, KFData* kfdata )
    {
        if ( kfentity->IsInited() )
        {
            static_cast< KFEntityEx* >( kfentity )->SyncRemoveData( kfdata, key );
        }

        // 开启保存数据库定时器
        if ( kfdata->GetDataSetting()->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            StartSaveEntityTimer( kfentity );
        }

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
            auto kfbind = _remove_data_function.Find( kfdata->GetName() );
            if ( kfbind != nullptr )
            {
                for ( auto& iter : kfbind->_objects )
                {
                    auto kffunction = iter.second;
                    kffunction->_function( kfentity, kfparent, key, kfdata );
                }
            }
        }
    }

    void KFComponentEx::StartSaveEntityTimer( KFEntity* kfentity )
    {
        // 不需要保存
        if ( !KFUtility::HaveBitMask< uint32 >( _entity_data_mask, __NEED_TO_SAVE__ ) )
        {
            return;
        }

        // 启动定时器
        if ( !kfentity->IsNeedToSave() )
        {
            kfentity->SetNeetToSave( true );

            static auto _delay_save_time_option = _kf_option->FindOption( __KF_STRING__( selaysavetime ) );
            __REGISTER_DELAY_TIMER__( kfentity->GetKeyID(), _delay_save_time_option->_uint32_value, &KFComponentEx::OnTimerSaveEntity );
        }
    }

    __KF_TIMER_FUNCTION__( KFComponentEx::OnTimerSaveEntity )
    {
        auto kfentity = FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        SaveEntity( kfentity, __FUNC_LINE__ );
    }

    void KFComponentEx::DeleteSaveEntity( KFEntity* kfentity )
    {
        if ( KFUtility::HaveBitMask< uint32 >( _entity_data_mask, __NEED_TO_SAVE__ ) )
        {
            // 删除定时器
            __UNREGISTER_OBJECT_TIMER__( kfentity->GetKeyID() );
            SaveEntity( kfentity, __FUNC_LINE__ );
        }
        else if ( KFUtility::HaveBitMask< uint32 >( _entity_data_mask, __DELETE_AND_SAVE__ ) )
        {
            SaveEntity( kfentity, __FUNC_LINE__ );
        }
        else if ( KFUtility::HaveBitMask< uint32 >( _entity_data_mask, __DELETE_AND_REMOVE__ ) )
        {
            DeleteEntity( kfentity );
        }
    }

    void KFComponentEx::SaveEntity( KFEntity* kfentity, const char* function, uint32 line )
    {
        if ( _entity_save_function != nullptr )
        {
            _entity_save_function( kfentity );
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