#include "KFTaskRefreshModule.hpp"
#include "KFProtocol/KFProtocol.h"
#include "KFTaskChainModule.hpp"

namespace KFrame
{
    void KFTaskRefreshModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_PLAYER_ENTER__( &KFTaskRefreshModule::OnEnterTaskRefreshModule );
        __REGISTER_PLAYER_LEAVE__( &KFTaskRefreshModule::OnLeaveTaskRefreshModule );

        __REGISTER_RESET__( __STRING__( taskrefresh ), &KFTaskRefreshModule::OnResetTaskRefresh );
        __REGISTER_ADD_ELEMENT__( __STRING__( taskrefresh ), &KFTaskRefreshModule::AddTaskRefreshElement );

    }

    void KFTaskRefreshModule::BeforeShut()
    {
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();

        __UN_RESET__( __STRING__( taskchain ) );
        __UN_ADD_ELEMENT__( __STRING__( taskrefresh ) );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFTaskRefreshModule::AddTaskRefreshElement )
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

    __KF_PLAYER_ENTER_FUNCTION__( KFTaskRefreshModule::OnEnterTaskRefreshModule )
    {
        UInt64List removes;
        auto kfrecord = player->Find( __STRING__( taskrefresh ) );
        for ( auto kftaskrefresh = kfrecord->First(); kftaskrefresh != nullptr; kftaskrefresh = kfrecord->Next() )
        {
            auto refreshid = kftaskrefresh->Get<uint32>( __STRING__( id ) );
            auto kfsetting = KFTaskRefreshConfig::Instance()->FindSetting( refreshid );
            if ( kfsetting == nullptr || kfsetting->_timer_refresh_time == _invalid_int )
            {
                removes.push_back( refreshid );
                continue;
            }

            auto lefttime = 0u;
            auto time = kftaskrefresh->Get<uint64>( __STRING__( time ) );
            if ( time > KFGlobal::Instance()->_real_time )
            {
                lefttime = ( time - KFGlobal::Instance()->_real_time );
            }
            else
            {
                // 计算最后一次循环的剩余时间
                lefttime = ( KFGlobal::Instance()->_real_time - time ) % kfsetting->_timer_refresh_time;

                // 重新附上最后一次循环
                StartRefreshTaskAndChain( player, kfsetting );
            }

            // 重新附上该有的任务链定时器
            __LOOP_TIMER_2__( player->GetKeyID(), refreshid, kfsetting->_timer_refresh_time * 1000u, lefttime * 1000u, &KFTaskRefreshModule::OnTimerTaskRefreshTimeout );
        }

        for ( auto refreshid : removes )
        {
            kfrecord->Remove( refreshid );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFTaskRefreshModule::OnLeaveTaskRefreshModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskRefreshModule::OpenRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist )
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
            auto kfrefreshsetting = KFTaskRefreshConfig::Instance()->FindSetting( iter.first );
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

            __LOOP_TIMER_2__( player->GetKeyID(), iter.first, timerrefeshtime * 1000u, 1u, &KFTaskRefreshModule::OnTimerTaskRefreshTimeout );
        }
    }

    void KFTaskRefreshModule::StopRefreshIdToLoop( KFEntity* player, const UInt32Map& idlist )
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
            player->RemoveRecord( __STRING__( taskrefresh ), iter.first );
        }
    }

    __KF_TIMER_FUNCTION__( KFTaskRefreshModule::OnTimerTaskRefreshTimeout )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kfrefreshsetting = KFTaskRefreshConfig::Instance()->FindSetting( subid );
        if ( kfrefreshsetting == nullptr )
        {
            return __LOG_ERROR__( "player=[{}] refreshid=[{}] can not find.", player->GetKeyID(), subid );
        }

        // 更新定时时间
        player->UpdateRecord( __STRING__( taskrefresh ), kfrefreshsetting->_id, __STRING__( time ),
                              KFEnum::Set, KFGlobal::Instance()->_real_time + kfrefreshsetting->_timer_refresh_time );
        ///////////////////////////////////////////////////////////////
        StartRefreshTaskAndChain( player, kfrefreshsetting );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_RESET_FUNCTION__( KFTaskRefreshModule::OnResetTaskRefresh )
    {
        for ( auto& iter : KFTaskRefreshConfig::Instance()->_reset_data_list._objects )
        {
            // 判断时间id是否相同
            auto kfrefreshdata = iter.second;
            if ( timeid != iter.first )
            {
                continue;
            }

            for ( auto kfsetting : kfrefreshdata->_refresh_list )
            {
                StartRefreshTaskAndChain( player, kfsetting );
            }
        }
    }

    void KFTaskRefreshModule::StartRefreshTaskAndChain( KFEntity* player, const KFTaskRefreshSetting* kfrefreshsetting )
    {
        if ( kfrefreshsetting->_task_chain_id != _invalid_int )
        {
            RefreshChain( player, kfrefreshsetting );
        }
        else
        {
            RefreshTask( player, kfrefreshsetting );
        }
    }

    void KFTaskRefreshModule::RefreshChain( KFEntity* player, const KFTaskRefreshSetting* kfrefreshsetting )
    {
        // 清除原来的任务
        static_cast<KFTaskChainModule*>( _kf_task_chain )->CleanTaskChain( player, kfrefreshsetting->_task_chain_id );

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
            static_cast< KFTaskChainModule* >( _kf_task_chain )->OpenTaskChain( player, kfrefreshsetting->_task_chain_id, 1u, kfrefreshsetting->_receive_time, kfrefreshsetting->_id );
        }
    }

    void KFTaskRefreshModule::RefreshTask( KFEntity* player, const KFTaskRefreshSetting* kfrefreshsetting )
    {
        // 清除所有的刷新任务
        CleanRefreshTask( player, kfrefreshsetting->_id );

        auto weightpool = KFWeightConfig::Instance()->FindWeightPool( kfrefreshsetting->_task_pool_id );
        if ( weightpool == nullptr )
        {
            return;
        }

        auto tasklist = weightpool->Rand( kfrefreshsetting->_task_count );
        for ( auto taskdata : tasklist )
        {
            _kf_task->OpenTask( player, taskdata->_id, kfrefreshsetting->_task_status, kfrefreshsetting->_done_time, 0u, 0u, kfrefreshsetting->_id );
        }
    }

    void KFTaskRefreshModule::CleanRefreshTask( KFEntity* player, uint32 refreshid )
    {
        auto kftaskrecord = player->Find( __STRING__( task ) );

        std::list< KFData* > tasklist;
        kftaskrecord->Find( __STRING__( refresh ), refreshid, tasklist, true );

        for ( auto kftask : tasklist )
        {
            auto taskid = kftask->Get<uint32>( __STRING__( id ) );
            player->RemoveRecord( kftaskrecord, taskid );
        }
    }
}