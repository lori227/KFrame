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

        __REGISTER_ENTER_PLAYER__( &KFTaskChainModule::OnEnterTaskChainModule );
        __REGISTER_LEAVE_PLAYER__( &KFTaskChainModule::OnLeaveTaskChainModule );

    }

    void KFTaskChainModule::BeforeShut()
    {
        __UN_RESET__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();

        __UN_ADD_ELEMENT__( __STRING__( taskchain ) );
        __UN_ADD_ELEMENT__( __STRING__( retaskchain ) );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFTaskChainModule::OnEnterTaskChainModule )
    {
        UInt64List removes;
        auto kfrecord = player->Find( __STRING__( retaskchain ) );
        for ( auto kfretaskchain = kfrecord->First(); kfretaskchain != nullptr; kfretaskchain = kfrecord->Next() )
        {
            auto refreshid = kfretaskchain->Get<uint32>( __STRING__( id ) );
            auto kfsetting = KFTaskChainRefreshConfig::Instance()->FindSetting( refreshid );
            // 定时刷新任务链
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
                OpenTaskChain( player, kfsetting->_task_chain_id, 1u, lefttime, kfsetting->_id, __FUNC_LINE__ );
            }

            // 重新附上该有的任务链定时器
            __LOOP_TIMER_2__( player->GetKeyID(), refreshid, kfsetting->_timer_refresh_time * 1000u, lefttime * 1000u, &KFTaskChainModule::OnTimerTaskChainRefreshTimeout );
        }

        for ( auto refreshid : removes )
        {
            kfrecord->Remove( refreshid );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFTaskChainModule::OnLeaveTaskChainModule )
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

        auto order = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( order ), 1.0f );
        OpenTaskChain( player, kfelementobject->_config_id, order, 0u, 0u, __FUNC_LINE__ );
        return true;
    }

    bool KFTaskChainModule::OpenTaskChain( KFEntity* player, uint32 taskchainid, uint32 order, uint32 time, uint32 refreshid, const char* function, uint32 line )
    {
        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( taskchainid );
        if ( kftaskchainsetting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskChainSettingNotExist, taskchainid );
            __LOG_ERROR_FUNCTION__( function, line, "taskchain=[{}] can't find setting", taskchainid );
            return false;
        }

        order = __MAX__( order, 1u );
        auto kftaskdatalist = kftaskchainsetting->_task_data_list.Find( order );
        if ( kftaskdatalist == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "taskchain=[{}] order=[{}] is error", taskchainid, order );
            return false;
        }

        uint64 timeout = 0u;
        if ( time != 0u )
        {
            timeout = KFGlobal::Instance()->_real_time + time;
        }

        return OpenTaskLogicDataList( player, kftaskdatalist, taskchainid, order, timeout, refreshid, function, line );
    }

    bool KFTaskChainModule::OpenTaskLogicDataList( KFEntity* player, const KFWeightList<KFTaskData>* taskdatalist, uint32 taskchainid, uint32 order, uint64 time, uint32 refreshid, const char* function, uint32 line )
    {
        auto taskdata = taskdatalist->Rand();
        if ( taskdata == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "taskchain=[{}] order=[{}] can't rand taskdata", taskchainid, order );
            return false;
        }

        // 开启逻辑功能
        UInt32List logicids;
        switch ( taskdata->_logic_type )
        {
        case KFEnum::Or:
        {
            auto count = taskdata->_logic_id_list.size();
            if ( count != 0u )
            {
                auto index = KFGlobal::Instance()->RandRatio( count );
                auto logicid = taskdata->_logic_id_list[ index ];
                auto ok = OpenTaskLogicData( player, taskchainid, order, taskdata, logicid, function, line );
                if ( ok )
                {
                    logicids.push_back( logicid );
                }
            }
        }
        break;
        case KFEnum::And:
        {
            for ( auto logicid : taskdata->_logic_id_list )
            {
                auto ok = OpenTaskLogicData( player, taskchainid, order, taskdata, logicid, function, line );
                if ( ok )
                {
                    logicids.push_back( logicid );
                }
            }
        }
        break;
        }

        // 开启任务
        auto kftask = _kf_task->OpenTask( player, taskdata->_id, taskdata->_task_status, time, refreshid, taskchainid, order, logicids );
        return kftask != nullptr;
    }

    void KFTaskChainModule::CleanTaskChain( KFEntity* player, uint32 taskchainid )
    {
        auto kftaskrecord = player->Find( __STRING__( task ) );

        std::list< KFData* > tasklist;
        kftaskrecord->Find( __STRING__( chain ), taskchainid, tasklist, true );

        for ( auto kftask : tasklist )
        {
            auto taskid = kftask->Get<uint32>( __STRING__( id ) );
            player->RemoveData( kftaskrecord, taskid );
        }
    }

    void KFTaskChainModule::StartRefreshTaskChain( KFEntity* player, const KFTaskChainRefreshSetting* kfrefreshsetting )
    {
        // 清除原来的任务
        CleanTaskChain( player, kfrefreshsetting->_task_chain_id );

        {
            // 判断条件
            if ( !_kf_condition->CheckCondition( player, &kfrefreshsetting->_conditions ) )
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
            OpenTaskChain( player, kfrefreshsetting->_task_chain_id, 1u, kfrefreshsetting->_receive_time, kfrefreshsetting->_id, __FUNC_LINE__ );
        }
    }

    void KFTaskChainModule::OpenExtendChain( KFEntity* player, const UInt32Map& chainlist, const char* function, uint32 line )
    {
        // 额外任务链id列表
        for ( auto& iter : chainlist )
        {
            // first=刷新Id second=万分比几率
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
            if ( rand >= iter.second )
            {
                continue;
            }

            OpenTaskChain( player, iter.first, 1u, 0u, 0u, function, line );
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
        OpenRefreshIdToLoop( player, idlist, __FUNC_LINE__ );
        return true;
    }

    void KFTaskChainModule::OpenRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist, const char* function, uint32 line )
    {
        // 刷新id 列表
        for ( auto& iter : idlist )
        {
            // first=刷新Id second=万分比几率
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
            if ( rand >= iter.second )
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

    void KFTaskChainModule::StopRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist, const char* function, uint32 line )
    {
        // 关闭id 列表
        for ( auto& iter : idlist )
        {
            // first=刷新Id second=万分比几率
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
            if ( rand >= iter.second )
            {
                continue;
            }

            __UN_TIMER_2__( player->GetKeyID(), iter.first );

            // 移除刷新记录
            player->RemoveData( __STRING__( retaskchain ), iter.first );
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
        player->UpdateData( __STRING__( retaskchain ), kfrefreshsetting->_id, __STRING__( time ),
                            KFEnum::Set, KFGlobal::Instance()->_real_time + kfrefreshsetting->_timer_refresh_time );
        ///////////////////////////////////////////////////////////////
        StartRefreshTaskChain( player, kfrefreshsetting );
    }

    void KFTaskChainModule::RemoveTaskChain( KFEntity* player, KFData* kftask )
    {
        auto taskchainid = kftask->Get<uint32>( __STRING__( chain ) );
        auto order = kftask->Get<uint32>( __STRING__( order ) );
        if ( taskchainid == 0u || order == 0u )
        {
            return;
        }

        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( taskchainid );
        if ( kftaskchainsetting == nullptr )
        {
            return;
        }

        // 附加的掉落组id
        auto taskid = kftask->Get<uint32>( __STRING__( id ) );
        auto taskdata = kftaskchainsetting->FindTaskData( order, taskid );
        if ( taskdata == nullptr )
        {
            return;
        }

        RemoveTaskLogicData( player, kftask, taskdata );
    }

    void KFTaskChainModule::FinishTaskChain( KFEntity* player, KFData* kftask, const char* function, uint32 line )
    {
        auto taskchainid = kftask->Get<uint32>( __STRING__( chain ) );
        auto order = kftask->Get<uint32>( __STRING__( order ) );
        auto time = kftask->Get( __STRING__( time ) );
        auto refreshid = kftask->Get( __STRING__( refresh ) );
        if ( taskchainid == 0u || order == 0u )
        {
            return;
        }

        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( taskchainid );
        if ( kftaskchainsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "player=[{}] taskchain=[{}] can't find setting", player->GetKeyID(), taskchainid );
        }

        // 附加的掉落组id
        auto taskid = kftask->Get<uint32>( __STRING__( id ) );
        auto taskdata = kftaskchainsetting->FindTaskData( order, taskid );
        if ( taskdata != nullptr )
        {
            // 开启额外任务链
            OpenExtendChain( player, taskdata->_extend_task_chain_list, function, line );

            // 开启循环刷新链
            OpenRefreshIdToLoop( player, taskdata->_start_refresh_id_list, function, line );

            // 关闭循环刷新链
            StopRefreshIdToLoop( player, taskdata->_stop_refresh_id_list, function, line );

            FinishTaskLogicData( player, kftask, taskdata );
        }

        ++order;
        auto kftaskdatalist = kftaskchainsetting->_task_data_list.Find( order );
        if ( kftaskdatalist == nullptr )
        {
            return;
        }

        OpenTaskLogicDataList( player, kftaskdatalist, taskchainid, order, time, refreshid, function, line );
    }

    bool KFTaskChainModule::OpenTaskLogicData( KFEntity* player, uint32 taskchainid, uint32 order, const KFTaskData* taskdata, uint32 logicid, const char* function, uint32 line )
    {
        if ( taskdata->_logic_name.empty() )
        {
            return false;
        }

        // 开启逻辑属性点
        auto kffunction = _open_task_chain_function.Find( taskdata->_logic_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( player, taskchainid, order, taskdata->_id, logicid, taskdata->_logic_status, function, line );
        }

        auto kfrecord = player->Find( taskdata->_logic_name );
        if ( kfrecord == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "taskchain=[{}] order=[{}] task=[{}] dataname=[{}] no class data", taskchainid, order, taskdata->_id, taskdata->_logic_name );
            return false;
        }

        player->UpdateData( kfrecord, logicid, kfrecord->_data_setting->_value_key_name, KFEnum::Set, taskdata->_logic_status );
        return true;
    }

    void KFTaskChainModule::FinishTaskLogicData( KFEntity* player, KFData* kftask, const KFTaskData* taskdata )
    {
        if ( taskdata->_logic_name.empty() )
        {
            return;
        }

        UInt32List logicids;

        // 结束逻辑属性点
        auto kffunction = _finish_task_chain_function.Find( taskdata->_logic_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( player, logicids, taskdata->_finish_status );
        }

        auto kfrecord = player->Find( taskdata->_logic_name );
        if ( kfrecord == nullptr )
        {
            return;
        }

        for ( auto logicid : logicids )
        {
            auto kfdata = kfrecord->Find( logicid );
            if ( kfdata != nullptr )
            {
                player->UpdateData( kfdata, kfrecord->_data_setting->_value_key_name, KFEnum::Set, taskdata->_finish_status );
            }
        }
    }

    void KFTaskChainModule::RemoveTaskLogicData( KFEntity* player, KFData* kftask, const KFTaskData* taskdata )
    {
        FinishTaskLogicData( player, kftask, taskdata );
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