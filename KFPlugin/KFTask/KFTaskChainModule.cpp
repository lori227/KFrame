#include "KFTaskChainModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFTaskChainModule::BeforeRun()
    {
        __REGISTER_RESET__( 0u, &KFTaskChainModule::OnResetRefreshTaskChain );

        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( taskchain ), &KFTaskChainModule::AddTaskChainElement );
        __REGISTER_ADD_ELEMENT__( __STRING__( retaskchain ), &KFTaskChainModule::AddTaskChainrefreshElement );

        __REGISTER_PLAYER_ENTER__( &KFTaskChainModule::OnEnterTaskChainModule );
        __REGISTER_PLAYER_LEAVE__( &KFTaskChainModule::OnLeaveTaskChainModule );

        __REGISTER_REMOVE_DATA_1__( __STRING__( task ), &KFTaskChainModule::OnRemoveTaskTaskChainModule );
    }

    void KFTaskChainModule::BeforeShut()
    {
        __UN_RESET__();
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();

        __UN_ADD_ELEMENT__( __STRING__( taskchain ) );
        __UN_ADD_ELEMENT__( __STRING__( retaskchain ) );
    }

    __KF_PLAYER_ENTER_FUNCTION__( KFTaskChainModule::OnEnterTaskChainModule )
    {
        UInt64List removes;
        auto kfrecord = player->Find( __STRING__( retaskchain ) );
        for ( auto kfretaskchain = kfrecord->First(); kfretaskchain != nullptr; kfretaskchain = kfrecord->Next() )
        {
            auto refreshid = kfretaskchain->Get<uint32>( __STRING__( id ) );
            auto kfsetting = KFTaskChainRefreshConfig::Instance()->FindSetting( refreshid );
            if ( kfsetting == nullptr || kfsetting->_timer_refresh_time == _invalid_int )
            {
                removes.push_back( refreshid );
                continue;
            }

            auto lefttime = 0u;
            auto time = kfretaskchain->Get<uint64>( __STRING__( time ) );
            if ( time > KFGlobal::Instance()->_real_time )
            {
                lefttime = ( time - KFGlobal::Instance()->_real_time );
            }
            else
            {
                // 计算最后一次循环的剩余时间
                lefttime = ( KFGlobal::Instance()->_real_time - time ) % kfsetting->_timer_refresh_time;

                // 重新附上最后一次循环
                OpenTaskChain( player, kfsetting->_task_chain_id, 1u, lefttime, kfsetting->_id );
            }

            // 重新附上该有的任务链定时器
            __LOOP_TIMER_2__( player->GetKeyID(), refreshid, kfsetting->_timer_refresh_time * 1000u, lefttime * 1000u, &KFTaskChainModule::OnTimerTaskChainRefreshTimeout );
        }

        for ( auto refreshid : removes )
        {
            kfrecord->Remove( refreshid );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFTaskChainModule::OnLeaveTaskChainModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
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
            player->RemoveRecordData( kftaskrecord, taskid );
        }
    }

    void KFTaskChainModule::StartRefreshTaskChain( KFEntity* player, const KFTaskChainRefreshSetting* kfrefreshsetting )
    {
        // 清除原来的任务
        CleanTaskChain( player, kfrefreshsetting->_task_chain_id );

        {
            // 判断条件
            if ( !_kf_condition->CheckStaticCondition( player, &kfrefreshsetting->_conditions ) )
            {
                return;
            }

            // 判断几率
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
            if ( rand >= kfrefreshsetting->_refresh_rate )
            {
                return;
            }

            // 开启新的任务链
            OpenTaskChain( player, kfrefreshsetting->_task_chain_id, 1u, kfrefreshsetting->_receive_time, kfrefreshsetting->_id );
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

    __KF_ADD_ELEMENT_FUNCTION__( KFTaskChainModule::AddTaskChainrefreshElement )
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

        UInt32Map idlist;
        idlist[ kfelementobject->_config_id ] = KFRandEnum::TenThousand;
        OpenRefreshIdToLoop( player, idlist );
        return true;
    }

    void KFTaskChainModule::OpenRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist )
    {
        // 刷新id 列表
        // first=刷新Id second=万分比几率
        for ( auto& iter : idlist )
        {
            if ( !KFGlobal::Instance()->RandCheck( KFRandEnum::TenThousand, iter.second ) )
            {
                continue;
            }

            auto refreshtid = iter.first;
            auto kfrefreshsetting = KFTaskChainRefreshConfig::Instance()->FindSetting( iter.first );
            if ( kfrefreshsetting == nullptr )
            {
                __LOG_ERROR__( "player=[{}] refreshid=[{}] can't find refresh setting", player->GetKeyID(), iter.first );
                continue;
            }

            auto timerrefeshtime = kfrefreshsetting->_timer_refresh_time;
            if ( timerrefeshtime == _invalid_int )
            {
                __LOG_ERROR__( "refreshid=[{}] be use timerrefeshtime == 0 ", iter.first );
                continue;
            }

            __LOOP_TIMER_2__( player->GetKeyID(), iter.first, timerrefeshtime * 1000u, 1u, &KFTaskChainModule::OnTimerTaskChainRefreshTimeout );
        }
    }

    void KFTaskChainModule::StopRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist )
    {
        // 关闭id 列表
        // first=刷新Id second=万分比几率
        for ( auto& iter : idlist )
        {
            if ( !KFGlobal::Instance()->RandCheck( KFRandEnum::TenThousand, iter.second ) )
            {
                continue;
            }

            __UN_TIMER_2__( player->GetKeyID(), iter.first );

            // 移除刷新记录
            player->RemoveRecordData( __STRING__( retaskchain ), iter.first );
        }
    }

    __KF_TIMER_FUNCTION__( KFTaskChainModule::OnTimerTaskChainRefreshTimeout )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kfrefreshsetting = KFTaskChainRefreshConfig::Instance()->FindSetting( subid );
        if ( kfrefreshsetting == nullptr )
        {
            return __LOG_ERROR__( "player=[{}] refreshid=[{}] can not find.", player->GetKeyID(), subid );
        }

        // 更新定时时间
        player->UpdateRecordData( __STRING__( retaskchain ), kfrefreshsetting->_id, __STRING__( time ),
                                  KFEnum::Set, KFGlobal::Instance()->_real_time + kfrefreshsetting->_timer_refresh_time );
        ///////////////////////////////////////////////////////////////
        StartRefreshTaskChain( player, kfrefreshsetting );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskChainModule::OnRemoveTaskTaskChainModule )
    {
        // 没有完成, 删除整个任务链
        auto status = kfdata->Get<uint32>( __STRING__( status ) );
        if ( status == KFMsg::ReceiveStatus )
        {
            FinishTaskChain( player, kfdata );
        }
        else
        {
            RemoveTaskChain( player, kfdata );
        }
    }

    void KFTaskChainModule::RemoveTaskChain( KFEntity* player, KFData* kftask )
    {
        auto chainid = kftask->Get<uint32>( __STRING__( chain ) );
        auto chainindex = kftask->Get<uint32>( __STRING__( index ) );
        if ( chainid == 0u || chainindex == 0u )
        {
            return;
        }

        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( chainid );
        if ( kftaskchainsetting == nullptr )
        {
            return;
        }

        // 附加的掉落组id
        auto taskid = kftask->Get<uint32>( __STRING__( id ) );
        auto taskdata = kftaskchainsetting->FindTaskData( chainindex, taskid );
        if ( taskdata == nullptr )
        {
            return;
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
            OpenRefreshIdToLoop( player, taskdata->_start_refresh_id_list );

            // 关闭循环刷新链
            StopRefreshIdToLoop( player, taskdata->_stop_refresh_id_list );
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
    __KF_RESET_FUNCTION__( KFTaskChainModule::OnResetRefreshTaskChain )
    {
        for ( auto& iter : KFTaskChainRefreshConfig::Instance()->_reset_data_list._objects )
        {
            auto kfrefreshdata = iter.second;
            if ( !_kf_reset->CheckResetTime( player, iter.first ) )
            {
                continue;
            }

            for ( auto kfsetting : kfrefreshdata->_refresh_list )
            {
                StartRefreshTaskChain( player, kfsetting );
            }
        }
    }
}