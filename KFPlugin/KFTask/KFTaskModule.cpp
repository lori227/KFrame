#include "KFTaskModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFTaskModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ADD_ELEMENT__( __STRING__( task ), &KFTaskModule::AddTaskElement );

        __REGISTER_ENTER_PLAYER__( &KFTaskModule::OnEnterTaskModule );
        __REGISTER_LEAVE_PLAYER__( &KFTaskModule::OnLeaveTaskModule );

        __REGISTER_ADD_DATA__( &KFTaskModule::OnAddDataTaskModule );
        __REGISTER_REMOVE_DATA__( &KFTaskModule::OnRemoveDataTaskModule );
        __REGISTER_UPDATE_DATA__( &KFTaskModule::OnUpdateDataTaskModule );
        __REGISTER_REMOVE_DATA_1__( __STRING__( task ), &KFTaskModule::OnRemoveTask );

        __REGISTER_EXECUTE__( __STRING__( task ), &KFTaskModule::OnExecuteUpdateTaskStatus );
        __REGISTER_EXECUTE__( __STRING__( taskcondition ), &KFTaskModule::OnExecuteUpdateTaskCondition );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_TASK_RECEIVE_REQ, &KFTaskModule::HandleTaskReceiveReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TASK_REWARD_REQ, &KFTaskModule::HandleTaskRewardReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TASK_REMOVE_REQ, &KFTaskModule::HandleTaskRemoveReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TASK_ADD_REQ, &KFTaskModule::HandleTaskAddReq );
    }

    void KFTaskModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();

        __UN_ADD_DATA__();
        __UN_REMOVE_DATA__();
        __UN_UPDATE_DATA__();
        __UN_REMOVE_DATA_1__( __STRING__( task ) );

        __UN_ADD_ELEMENT__( __STRING__( task ) );
        __UN_EXECUTE__( __STRING__( task ) );
        __UN_EXECUTE__( __STRING__( taskcondition ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_TASK_RECEIVE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TASK_REWARD_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TASK_REMOVE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TASK_ADD_REQ );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFTaskModule::AddTaskElement )
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

        // 任务状态
        auto status = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( status ), 1.0f );
        if ( status == KFMsg::InitStatus )
        {
            __LOG_ERROR_FUNCTION__( function, line, "task id=[{}] status = 0", kfelementobject->_config_id );
            return false;
        }

        auto taskchainid = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( chain ), 1.0f );
        auto order = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( order ), 1.0f );
        auto time = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( time ), 1.0f );
        if ( time != 0u )
        {
            time += KFGlobal::Instance()->_real_time;
        }

        UInt32List logicids;
        OpenTask( player, kfelementobject->_config_id, status, time, 0u, taskchainid, order, logicids );
        return true;
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskModule::OnRemoveTask )
    {
        auto status = kfdata->Get<uint32>( __STRING__( status ) );
        if ( status == KFMsg::ReceiveStatus )
        {
            return;
        }

        _kf_task_chain->RemoveTaskChain( player, kfdata );
        auto time = kfdata->Get<uint32>( __STRING__( time ) );
        if ( time != 0u )
        {
            __UN_TIMER_2__( player->GetKeyID(), key );
        }
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFTaskModule::OnEnterTaskModule )
    {
        UInt64List removes;
        std::map<KFData*, const KFTaskSetting* > finishtasklist;

        auto kftaskrecord = player->Find( __STRING__( task ) );
        for ( auto kftask = kftaskrecord->First(); kftask != nullptr; kftask = kftaskrecord->Next() )
        {
            auto taskid = kftask->Get<uint32>( __STRING__( id ) );
            auto kfsetting = KFTaskConfig::Instance()->FindSetting( taskid );
            if ( kfsetting == nullptr )
            {
                removes.push_back( taskid );
                continue;
            }

            auto status = kftask->Get<uint32>( __STRING__( status ) );
            if ( status == KFMsg::DoneStatus )
            {
                finishtasklist[ kftask ] = kfsetting;
                continue;
            }

            // 超时定时器
            auto time = kftask->Get( __STRING__( time ) );
            if ( time != 0u )
            {
                StartTaskTimeoutTimer( player, taskid, time );
            }
        }

        for ( auto taskid : removes )
        {
            kftaskrecord->Remove( taskid );
        }

        for ( auto& iter : finishtasklist )
        {
            AddFinishTask( player, iter.first, iter.second, true );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFTaskModule::OnLeaveTaskModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    KFData* KFTaskModule::OpenTask( KFEntity* player, uint32 taskid, uint32 status, uint64 time )
    {
        auto kfsetting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR__( "task=[{}] can't find setting", taskid );
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );
            return nullptr;
        }

        return OpenTask( player, kfsetting, status, time, 0u );
    }

    KFData* KFTaskModule::OpenTask( KFEntity* player, const KFTaskSetting* kfsetting, uint32 status, uint64 time, uint32 refreshid )
    {
        auto kftaskrecord = player->Find( __STRING__( task ) );
        auto kftask = kftaskrecord->Find( kfsetting->_id );
        if ( kftask == nullptr )
        {
            kftask = player->CreateData( kftaskrecord );
            kftask->Set( __STRING__( status ), status );
            kftask->Set( __STRING__( refresh ), refreshid );
            SetTaskTime( player, kftask, kfsetting, time, false );

            // 前置条件
            auto kfpreconditionobject = kftask->Find( __STRING__( preconditions ) );
            _kf_condition->AddCondition( player, kfpreconditionobject, kfsetting->_pre_condition, kfsetting->_pre_condition_type );
            _kf_condition->InitCondition( player, kfpreconditionobject, KFConditionEnum::LimitNull, false );

            // 完成条件
            auto kfconditionobject = kftask->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, kfconditionobject, kfsetting->_complete_condition, kfsetting->_complete_condition_type );
            InitTaskCondition( player, kfsetting, kftask, status, false );

            player->AddData( kftaskrecord, kfsetting->_id, kftask );
        }
        else
        {
            // 更新状态
            UpdataTaskStatus( player, kfsetting, kftask, status, time );
        }

        return kftask;
    }

    KFData* KFTaskModule::OpenTask( KFEntity* player, uint32 taskid, uint32 status, uint64 time, uint32 refreshid, uint32 taskchainid, uint32 order, const UInt32List& logicids )
    {
        auto kfsetting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR__( "task=[{}] can't find setting", taskid );
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );
            return nullptr;
        }

        auto kftask = OpenTask( player, kfsetting, status, time, refreshid );
        if ( kftask != nullptr )
        {
            kftask->Set( __STRING__( order ), order );
            kftask->Set( __STRING__( chain ), taskchainid );
        }

        return kftask;
    }

    void KFTaskModule::UpdataTaskStatus( KFEntity* player, const KFTaskSetting* kfsetting, KFData* kftask, uint32 status, uint64 time )
    {
        auto laststatus = kftask->Get<uint32>( __STRING__( status ) );
        if ( laststatus == status )
        {
            return;
        }

        // 更新状态
        player->UpdateData( kftask, __STRING__( status ), KFEnum::Set, status );

        // 设置时间
        SetTaskTime( player, kftask, kfsetting, time, true );

        // 初始化条件
        InitTaskCondition( player, kfsetting, kftask, status, true );
    }

    void KFTaskModule::SetTaskTime( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting, uint64 time, bool update )
    {
        if ( time == 0u )
        {
            return;
        }

        if ( !update )
        {
            kftask->Set( __STRING__( time ), time );
        }
        else
        {
            player->UpdateData( kftask, __STRING__( time ), KFEnum::Set, time );
        }

        // 启动定时器
        StartTaskTimeoutTimer( player, kfsetting->_id, time );
    }

    void KFTaskModule::AddFinishTask( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting, bool update )
    {
        // 不是自动提交
        if ( !kfsetting->IsAutoTask() )
        {
            return;
        }

        if ( update )
        {
            FinishTask( player, kftask, kfsetting );
        }
        else
        {
            // 新加的任务, 启动一个定时器
            __LIMIT_TIMER_2__( player->GetKeyID(), kfsetting->_id, 10u, 1, &KFTaskModule::OnTimerTaskFinish );
        }
    }

    void KFTaskModule::FinishTask( KFEntity* player, uint32 taskid )
    {
        auto kftask = player->Find( __STRING__( task ), taskid );
        if ( kftask == nullptr )
        {
            return;
        }

        auto kfsetting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );
        }

        FinishTask( player, kftask, kfsetting );
    }

    void KFTaskModule::FinishTask( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting )
    {
        // 设置成已经领取状态, 防止删除失败后还能领取
        kftask->Set<uint32>( __STRING__( type ), kfsetting->_type );

        // 更新状态
        player->UpdateData( kftask, __STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );

        // 任务输出执行
        TaskExecute( player, kftask, kfsetting );

        // 提示客户端
        _kf_display->DelayToClient( player, KFMsg::TaskRewardOk, kfsetting->_id );

        // 完成任务链
        _kf_task_chain->FinishTaskChain( player, kftask, __FUNC_LINE__ );

        // 删除任务
        player->RemoveData( kftask->GetParent(), kfsetting->_id );
    }

    void KFTaskModule::TaskExecute( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting )
    {
        auto executedata = &kfsetting->_execute_data;
        if ( executedata->_name == __STRING__( drop ) )
        {
            if ( executedata->_param_list._params.size() >= 3u )
            {
                const_cast< KFExecuteData* >( executedata )->_calc_value = 0u;
                auto conditionid = executedata->_param_list._params[ 1 ]->_int_value;
                if ( conditionid != 0u )
                {
                    auto kfconditionobject = kftask->Find( __STRING__( conditions ) );
                    auto value = kfconditionobject->Get<uint32>( __STRING__( condition ), conditionid, __STRING__( value ) );
                    const_cast< KFExecuteData* >( executedata )->_calc_value = value;
                }
            }
        }

        _kf_execute->Execute( player, executedata, __STRING__( task ), kfsetting->_id, __FUNC_LINE__ );
    }

    void KFTaskModule::StartTaskTimeoutTimer( KFEntity* player, uint32 taskid, uint64 time )
    {
        auto lefttime = 1u;
        if ( time > KFGlobal::Instance()->_real_time )
        {
            lefttime = ( time - KFGlobal::Instance()->_real_time ) * 1000u;
        }

        __LIMIT_TIMER_2__( player->GetKeyID(), taskid, lefttime, 1, &KFTaskModule::OnTimerTaskTimeout );
    }

    __KF_TIMER_FUNCTION__( KFTaskModule::OnTimerTaskTimeout )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        player->RemoveData( __STRING__( task ), subid );
    }

    void KFTaskModule::StopTaskTimeoutTimer( KFEntity* player, KFData* kftask, uint32 taskid )
    {
        auto time = kftask->Get( __STRING__( time ) );
        if ( time != 0u )
        {
            __UN_TIMER_2__( player->GetKeyID(), taskid );
            player->UpdateData( kftask, __STRING__( time ), KFEnum::Set, 0u );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskModule::InitTaskCondition( KFEntity* player, const KFTaskSetting* kfsetting, KFData* kftask, uint32 status, bool update )
    {
        // 不在领取状态, 不初始化任务条件
        if ( status != KFMsg::ExecuteStatus )
        {
            return;
        }

        // 地点判断
        uint32 limitmask = KFConditionEnum::LimitNull;
        if ( !_kf_condition->CheckCondition( player, &kfsetting->_place_condition ) )
        {
            KFUtility::AddBitMask<uint32>( limitmask, KFConditionEnum::LimitPlace );
        }

        // 初始化条件
        auto kfconditionobject = kftask->Find( __STRING__( conditions ) );
        auto complete = _kf_condition->InitCondition( player, kfconditionobject, limitmask, update );
        if ( complete )
        {
            DoneTask( player, kftask, kfsetting, update );
        }

        _kf_display->DelayToClient( player, KFMsg::TaskReceiveOk, kfsetting->_id );
    }

    void KFTaskModule::DoneTask( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting, bool update )
    {
        auto kfstatus = kftask->Find( __STRING__( status ) );
        auto status = kfstatus->Get<uint32>();
        if ( status == KFMsg::DoneStatus || status == KFMsg::ReceiveStatus )
        {
            return;
        }

        // 任务完成, 停止倒计时
        StopTaskTimeoutTimer( player, kftask, kfsetting->_id );
        if ( !update )
        {
            kfstatus->Set<uint32>( KFMsg::DoneStatus );
        }
        else
        {
            player->UpdateData( kfstatus, KFEnum::Set, KFMsg::DoneStatus );
        }

        // 刚创建的任务, 如果是完成状态, 又是自动交付, 延迟删除
        AddFinishTask( player, kftask, kfsetting, update );
    }

    __KF_TIMER_FUNCTION__( KFTaskModule::OnTimerTaskFinish )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        FinishTask( player, subid );
    }

    uint32 KFTaskModule::CheckTaskUpdateStatus( KFData* kftask, const KFTaskSetting* kfsetting )
    {
        auto status = kftask->Get<uint32>( __STRING__( status ) );
        if ( status == KFMsg::ExecuteStatus )
        {
            return KFConditionEnum::LimitNull;
        }

        if ( status == KFMsg::DoneStatus )
        {
            if ( kfsetting->_execute_data._param_list._params.size() >= 2u )
            {
                auto conditionid = kfsetting->_execute_data._param_list._params[ 2 ]->_int_value;
                if ( conditionid != 0u )
                {
                    return KFConditionEnum::LimitStatus;
                }
            }
        }

        return KFConditionEnum::LimitStop;
    }

    uint32 KFTaskModule::CheckTaskCanUpdate( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting )
    {
        auto limitmask = CheckTaskUpdateStatus( kftask, kfsetting );
        if ( limitmask == KFConditionEnum::LimitStop )
        {
            return limitmask;
        }

        // 不满足地点条件
        if ( !_kf_condition->CheckCondition( player, &kfsetting->_place_condition ) )
        {
            KFUtility::AddBitMask<uint32>( limitmask, KFConditionEnum::LimitPlace );
        }

        return limitmask;
    }

    class KFUpdateTaskData
    {
    public:
        KFData* _task;
        const KFTaskSetting* _kfsetting = nullptr;
        uint32 _limit_mask = KFConditionEnum::LimitNull;
    };

#define __GET_UPDATA_TASK_LIST__()\
    std::list< KFUpdateTaskData > _pre_task;\
    std::list< KFUpdateTaskData > _update_task;\
    auto kftaskrecord = player->Find( __STRING__( task ) );\
    for ( auto kftask = kftaskrecord->First(); kftask != nullptr; kftask = kftaskrecord->Next() )\
    {\
        auto taskid = kftask->Get<uint32>( __STRING__( id ) );\
        auto kfsetting = KFTaskConfig::Instance()->FindSetting( taskid );\
        if ( kfsetting == nullptr )\
        {\
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );\
            continue;\
        }\
        auto status = kftask->Get<uint32>( __STRING__( status ) );\
        if ( status == KFMsg::ActiveStatus )\
        {\
            KFUpdateTaskData updatedata;\
            updatedata._task = kftask; \
            updatedata._kfsetting = kfsetting; \
            _pre_task.push_back( updatedata );\
        }\
        else\
        {\
            auto limitmask = CheckTaskCanUpdate( player, kftask, kfsetting ); \
            if ( limitmask != KFConditionEnum::LimitStop )\
            {\
                KFUpdateTaskData updatedata; \
                updatedata._task = kftask; \
                updatedata._kfsetting = kfsetting; \
                updatedata._limit_mask = limitmask; \
                _update_task.push_back( updatedata ); \
            }\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFTaskModule::OnAddDataTaskModule )
    {
        __GET_UPDATA_TASK_LIST__();
        for ( auto& data : _pre_task )
        {
            auto kftask = data._task;
            auto kfsetting = data._kfsetting;
            auto limitmask = data._limit_mask;

            auto kfconditionobject = kftask->Find( __STRING__( preconditions ) );
            _kf_condition->UpdateAddCondition( player, kfconditionobject, limitmask, kfdata );
        }

        for ( auto& data : _update_task )
        {
            auto kftask = data._task;
            auto kfsetting = data._kfsetting;
            auto limitmask = data._limit_mask;

            auto kfconditionobject = kftask->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateAddCondition( player, kfconditionobject, limitmask, kfdata );
            if ( complete )
            {
                DoneTask( player, kftask, kfsetting, true );
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFTaskModule::OnRemoveDataTaskModule )
    {
        __GET_UPDATA_TASK_LIST__();
        for ( auto& data : _pre_task )
        {
            auto kftask = data._task;
            auto kfsetting = data._kfsetting;
            auto limitmask = data._limit_mask;

            auto kfconditionobject = kftask->Find( __STRING__( preconditions ) );
            _kf_condition->UpdateRemoveCondition( player, kfconditionobject, limitmask, kfdata );
        }

        for ( auto& data : _update_task )
        {
            auto kftask = data._task;
            auto kfsetting = data._kfsetting;
            auto limitmask = data._limit_mask;

            auto kfconditionobject = kftask->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateRemoveCondition( player, kfconditionobject, limitmask, kfdata );
            if ( complete )
            {
                DoneTask( player, kftask, kfsetting, true );
            }
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateDataTaskModule )
    {
        __GET_UPDATA_TASK_LIST__();
        for ( auto& data : _pre_task )
        {
            auto kftask = data._task;
            auto kfsetting = data._kfsetting;
            auto limitmask = data._limit_mask;

            auto kfconditionobject = kftask->Find( __STRING__( preconditions ) );
            _kf_condition->UpdateUpdateCondition( player, kfconditionobject, limitmask, kfdata, operate, value, newvalue );
        }

        for ( auto& data : _update_task )
        {
            auto kftask = data._task;
            auto kfsetting = data._kfsetting;
            auto limitmask = data._limit_mask;

            auto kfconditionobject = kftask->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateUpdateCondition( player, kfconditionobject, limitmask, kfdata, operate, value, newvalue );
            if ( complete )
            {
                DoneTask( player, kftask, kfsetting, true );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskAddReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTaskAddReq );

        OpenTask( player, kfmsg.id(), KFMsg::ExecuteStatus, 0u );
    }

    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskReceiveReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTaskReceiveReq );

        auto kfsetting = KFTaskConfig::Instance()->FindSetting( kfmsg.id() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, kfmsg.id() );
        }

        auto kftask = player->Find( __STRING__( task ), kfmsg.id() );
        if ( kftask == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskNotActive );
        }

        auto status = kftask->Get<uint32>( __STRING__( status ) );
        if ( status != KFMsg::ActiveStatus )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskAlreadyReceive );
        }

        // 判断前置条件
        auto kfconditionobject = kftask->Find( __STRING__( preconditions ) );
        auto ok = _kf_condition->CheckCondition( player, kfconditionobject );
        if ( !ok )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskNotPreCondition );
        }

        // 判断任务链的接取消耗
        auto timeout = 0u;
        auto refreshid = kftask->Get<uint32>( __STRING__( refresh ) );
        auto kfrefreshsetting = KFTaskChainRefreshConfig::Instance()->FindSetting( refreshid );
        if ( kfrefreshsetting != nullptr )
        {
            // 判断接取消耗
            auto& dataname = player->RemoveElement( &kfrefreshsetting->_receive_cost, _default_multiple, __STRING__( taskreceive ), kfsetting->_id, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
            }

            if ( kfrefreshsetting->_done_time != 0u )
            {
                timeout = KFGlobal::Instance()->_real_time + kfrefreshsetting->_done_time;
            }
        }

        UpdataTaskStatus( player, kfsetting, kftask, KFMsg::ExecuteStatus, timeout );
    }

    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTaskRewardReq );

        auto kfsetting = KFTaskConfig::Instance()->FindSetting( kfmsg.id() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, kfmsg.id() );
        }

        auto kftask = player->Find( __STRING__( task ), kfmsg.id() );
        if ( kftask == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskCanNotFindData );
        }

        // 不是完成状态
        auto taskstatus = kftask->Get<uint32>( __STRING__( status ) );
        if ( taskstatus != KFMsg::DoneStatus )
        {
            if ( taskstatus == KFMsg::ReceiveStatus )
            {
                return _kf_display->SendToClient( player, KFMsg::TaskAlreadyReward );
            }
            else
            {
                return _kf_display->SendToClient( player, KFMsg::TaskNotDone );
            }
        }

        // 交付完成任务
        FinishTask( player, kftask, kfsetting );
    }

    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskRemoveReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTaskRemoveReq );

        auto kfsetting = KFTaskConfig::Instance()->FindSetting( kfmsg.id() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, kfmsg.id() );
        }

        player->RemoveData( __STRING__( task ), kfmsg.id() );
    }

    __KF_EXECUTE_FUNCTION__( KFTaskModule::OnExecuteUpdateTaskStatus )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            return false;
        }

        auto taskid = executedata->_param_list._params[ 0 ]->_int_value;
        auto status = executedata->_param_list._params[ 1 ]->_int_value;
        auto kfsetting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( kfsetting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );
            return false;
        }

        auto kftaskrecord = player->Find( __STRING__( task ) );
        switch ( status )
        {
        case KFMsg::InitStatus:
        {
            player->RemoveData( kftaskrecord, taskid );
        }
        break;
        case KFMsg::ActiveStatus:
        {
            auto kftask = kftaskrecord->Find( taskid );
            if ( kftask != nullptr )
            {
                auto timeout = 0u;
                auto refreshid = kftaskrecord->Get( taskid, __STRING__( refresh ) );
                if ( refreshid != 0u )
                {
                    auto kfrefreshsetting = KFTaskChainRefreshConfig::Instance()->FindSetting( refreshid );
                    if ( kfrefreshsetting != nullptr && kfrefreshsetting->_receive_time != 0u )
                    {
                        timeout = KFGlobal::Instance()->_real_time + kfrefreshsetting->_receive_time;
                    }
                }

                // 更新状态
                UpdataTaskStatus( player, kfsetting, kftask, status, timeout );
            }
            else
            {
                // 开启任务
                OpenTask( player, kfsetting, status, 0u, 0u );
            }
        }
        break;
        case KFMsg::ExecuteStatus:
        {
            auto timeout = 0u;
            auto refreshid = kftaskrecord->Get( taskid, __STRING__( refresh ) );
            if ( refreshid != 0u )
            {
                auto kfrefreshsetting = KFTaskChainRefreshConfig::Instance()->FindSetting( refreshid );
                if ( kfrefreshsetting != nullptr && kfrefreshsetting->_done_time != 0u )
                {
                    timeout = KFGlobal::Instance()->_real_time + kfrefreshsetting->_done_time;
                }
            }

            // 开启任务
            OpenTask( player, kfsetting, status, timeout, 0u );
        }
        break;
        case KFMsg::DoneStatus:
        {
            auto kftask = kftaskrecord->Find( taskid );
            if ( kftask != nullptr )
            {
                DoneTask( player, kftask, kfsetting, true );
            }
        }
        break;
        default:
            break;
        }

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFTaskModule::OnExecuteUpdateTaskCondition )
    {
        if ( executedata->_param_list._params.size() < 4u )
        {
            return false;
        }

        auto taskid = executedata->_param_list._params[ 0 ]->_int_value;
        auto conditionid = executedata->_param_list._params[ 1 ]->_int_value;
        auto operate = executedata->_param_list._params[ 2 ]->_int_value;
        auto conditionvalue = executedata->_param_list._params[ 3 ]->_int_value;
        auto kfsetting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( kfsetting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );
            return false;
        }

        auto kftask = player->Find( __STRING__( task ), taskid );
        if ( kftask != nullptr )
        {
            auto status = kftask->Get<uint32>( __STRING__( status ) );
            if ( status == KFMsg::ExecuteStatus )
            {
                auto kfconditionobject = kftask->Find( __STRING__( conditions ) );
                auto ok = _kf_condition->UpdateCondition( player, kfconditionobject, conditionid, operate, conditionvalue );
                if ( ok )
                {
                    DoneTask( player, kftask, kfsetting, true );
                }
            }
        }

        return true;
    }
}