#include "KFTaskChainModule.hpp"
#include "KFProtocol/KFProtocol.h"
#include "KFTaskRefreshModule.hpp"

namespace KFrame
{
    void KFTaskChainModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( taskchain ), &KFTaskChainModule::AddTaskChainElement );
        __REGISTER_REMOVE_DATA_1__( __STRING__( task ), &KFTaskChainModule::OnRemoveTaskTaskChainModule );
    }

    void KFTaskChainModule::BeforeShut()
    {
        __UN_ADD_ELEMENT__( __STRING__( taskchain ) );
        __UN_REMOVE_DATA_1__( __STRING__( task ) );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskChainModule::BindOpenTaskChainFunction( const std::string& name, KFOpenTaskChainFunction& function )
    {
        auto kffucntion = _open_task_chain_function.Create( name );
        kffucntion->_function = function;
    }

    void KFTaskChainModule::UnBindOpenTaskChainFunction( const std::string& name )
    {
        _open_task_chain_function.Remove( name );
    }

    void KFTaskChainModule::BindFinishTaskChainFunction( const std::string& name, KFFinishTaskChainFunction& function )
    {
        auto kffucntion = _finish_task_chain_function.Create( name );
        kffucntion->_function = function;
    }

    void KFTaskChainModule::UnBindFinishTaskChainFunction( const std::string& name )
    {
        _finish_task_chain_function.Remove( name );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFTaskChainModule::AddTaskChainElement )
    {
        auto kfelement = kfresult->_element;
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

        auto chainindex = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( index ) );
        OpenTaskChain( player, kfelementobject->_config_id, chainindex, 0u, 0u );
        return true;
    }

    bool KFTaskChainModule::OpenTaskChain( KFEntity* player, uint32 chainid, uint32 chainindex, uint32 validtime, uint32 refreshid )
    {
        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( chainid );
        if ( kftaskchainsetting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskChainSettingNotExist, chainid );
            __LOG_ERROR__( "taskchain=[{}] can't find setting", chainid );
            return false;
        }

        chainindex = __MAX__( chainindex, 1u );
        auto kftaskdatalist = kftaskchainsetting->_task_data_list.Find( chainindex );
        if ( kftaskdatalist == nullptr )
        {
            __LOG_ERROR__( "taskchain=[{}] order=[{}] is error", chainid, chainindex );
            return false;
        }

        return OpenTaskLogicDataList( player, kftaskdatalist, chainid, chainindex, validtime, refreshid );
    }

    bool KFTaskChainModule::OpenTaskLogicDataList( KFEntity* player, const KFWeightList<KFTaskData>* taskdatalist, uint32 chainid, uint32 chainindex, uint32 validtime, uint32 refreshid )
    {
        auto taskdata = taskdatalist->Rand();
        if ( taskdata == nullptr )
        {
            __LOG_ERROR__( "taskchain=[{}] order=[{}] can't rand taskdata", chainid, chainindex );
            return false;
        }

        // 开启任务
        auto kftask = _kf_task->OpenTask( player, taskdata->_id, taskdata->_task_status, validtime, chainid, chainindex, refreshid );
        return kftask != nullptr;
    }

    void KFTaskChainModule::CleanTaskChain( KFEntity* player, uint32 chainid )
    {
        auto kftaskrecord = player->Find( __STRING__( task ) );

        std::list< KFData* > tasklist;
        kftaskrecord->Find( __STRING__( chain ), chainid, tasklist, true );

        for ( auto kftask : tasklist )
        {
            auto taskid = kftask->Get<uint32>( __STRING__( id ) );
            player->RemoveRecord( kftaskrecord, taskid );
        }
    }

    void KFTaskChainModule::OpenExtendChain( KFEntity* player, const UInt32Map& chainlist )
    {
        // 额外任务链id列表
        // first=刷新Id second=万分比几率
        for ( auto& iter : chainlist )
        {
            if ( KFGlobal::Instance()->RandCheck( KFRandEnum::TenThousand, iter.second ) )
            {
                OpenTaskChain( player, iter.first, 1u, 0u, 0u );
            }
        }
    }

    bool KFTaskChainModule::IsTaskChain( KFEntity* player, KFData* kftask )
    {
        auto taskchainid = kftask->Get<uint32>( __STRING__( chain ) );
        auto chainindex = kftask->Get<uint32>( __STRING__( index ) );
        return taskchainid != 0u && chainindex != 0u;
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskChainModule::OnRemoveTaskTaskChainModule )
    {
        // 没有完成, 删除整个任务链
        auto status = kfdata->Get<uint32>( __STRING__( status ) );
        if ( status == KFMsg::ReceiveStatus )
        {
            FinishTaskChain( player, kfdata );
        }
    }

    void KFTaskChainModule::FinishTaskChain( KFEntity* player, KFData* kftask )
    {
        auto taskchainid = kftask->Get<uint32>( __STRING__( chain ) );
        auto chainindex = kftask->Get<uint32>( __STRING__( index ) );
        auto validtime = kftask->Get( __STRING__( time ) );
        auto refreshid = kftask->Get( __STRING__( refresh ) );
        if ( taskchainid == 0u || chainindex == 0u )
        {
            return;
        }

        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( taskchainid );
        if ( kftaskchainsetting == nullptr )
        {
            return __LOG_ERROR__( "player=[{}] taskchain=[{}] can't find setting", player->GetKeyID(), taskchainid );
        }

        // 附加的掉落组id
        auto taskid = kftask->Get<uint32>( __STRING__( id ) );
        auto taskdata = kftaskchainsetting->FindTaskData( chainindex, taskid );
        if ( taskdata != nullptr )
        {
            // 开启额外任务链
            OpenExtendChain( player, taskdata->_extend_task_chain_list );

            // 开启循环刷新链
            static_cast<KFTaskRefreshModule*>( _kf_task_refresh )->OpenRefreshIdToLoop( player, taskdata->_start_refresh_id_list );

            // 关闭循环刷新链
            static_cast< KFTaskRefreshModule* >( _kf_task_refresh )->StopRefreshIdToLoop( player, taskdata->_stop_refresh_id_list );
        }

        ++chainindex;
        auto kftaskdatalist = kftaskchainsetting->_task_data_list.Find( chainindex );
        if ( kftaskdatalist == nullptr )
        {
            return;
        }

        OpenTaskLogicDataList( player, kftaskdatalist, taskchainid, chainindex, validtime, refreshid );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}