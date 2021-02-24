#include "KFTaskModule.hpp"

namespace KFrame
{
    namespace KFTaskEnum
    {
        enum MyEnum
        {
            CompleteRequest = 1,	// 玩家请求交付
            ComplelteAuto = 2,		// 自动交付
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( task ), &KFTaskModule::AddTaskElement );

        __REGISTER_PLAYER_ENTER__( &KFTaskModule::OnEnterTaskModule );
        __REGISTER_PLAYER_LEAVE__( &KFTaskModule::OnLeaveTaskModule );

        __REGISTER_ADD_DATA__( &KFTaskModule::OnAddDataTaskModule );
        __REGISTER_REMOVE_DATA__( &KFTaskModule::OnRemoveDataTaskModule );
        __REGISTER_UPDATE_DATA__( &KFTaskModule::OnUpdateDataTaskModule );

        __REGISTER_REMOVE_DATA_1__( __STRING__( task ), &KFTaskModule::OnRemoveTask );

        __REGISTER_EXECUTE__( __STRING__( task ), &KFTaskModule::OnExecuteUpdateTaskStatus );
        __REGISTER_EXECUTE__( __STRING__( taskcondition ), &KFTaskModule::OnExecuteUpdateTaskCondition );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TASK_RECEIVE_REQ, &KFTaskModule::HandleTaskReceiveReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TASK_REWARD_REQ, &KFTaskModule::HandleTaskRewardReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TASK_REMOVE_REQ, &KFTaskModule::HandleTaskRemoveReq );
    }

    void KFTaskModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();

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
        auto status = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( status ) );
        auto chainid = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( chain ) );
        auto chainindex = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( index ) );
        auto validtime = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( validtime ) );
        OpenTask( player, kfelementobject->_config_id, status, validtime, chainid, chainindex );
        return true;
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskModule::OnRemoveTask )
    {
        // 如果存在定时器
        auto time = kfdata->Get<uint32>( __STRING__( time ) );
        if ( time != 0u )
        {
            __UN_TIMER_2__( player->GetKeyID(), key );
        }
    }

    __KF_PLAYER_ENTER_FUNCTION__( KFTaskModule::OnEnterTaskModule )
    {
        UInt64List removes;
        std::map<DataPtr, const KFTaskSetting*> finishtasklist;

        auto kftaskrecord = player->Find( __STRING__( task ) );
        for ( auto kftask = kftaskrecord->First(); kftask != nullptr; kftask = kftaskrecord->Next() )
        {
            auto taskid = kftask->Get<uint32>( __STRING__( id ) );
            auto setting = KFTaskConfig::Instance()->FindSetting( taskid );
            if ( setting == nullptr )
            {
                removes.push_back( taskid );
                continue;
            }

            auto status = kftask->Get<uint32>( __STRING__( status ) );
            if ( status == KFMsg::DoneStatus )
            {
                finishtasklist[ kftask ] = setting;
                continue;
            }

            // 超时定时器
            auto time = kftask->Get( __STRING__( time ) );
            if ( time != 0u )
            {
                StartTaskTimeoutTimer( player, taskid, time );
            }
        }

        // 删除配置错误的任务
        for ( auto taskid : removes )
        {
            kftaskrecord->Remove( taskid );
        }

        // 出来已经完成的任务
        for ( auto& iter : finishtasklist )
        {
            AddFinishTask( player, iter.first, iter.second, true );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFTaskModule::OnLeaveTaskModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    DataPtr KFTaskModule::OpenTask( EntityPtr player, uint32 taskid, uint32 status, uint64 validtime /* = 0u */,
                                    uint32 chainid /* = 0u */, uint32 chainindex /* = 1u */ )
    {
        auto setting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "task=[{}] can't find setting", taskid );
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );
            return nullptr;
        }

        auto kftask = CreateTask( player, setting, status, validtime );
        kftask->Set( __STRING__( chain ), chainid );
        kftask->Set( __STRING__( index ), chainindex );
        return kftask;
    }

    DataPtr KFTaskModule::CreateTask( EntityPtr player, const KFTaskSetting* setting, uint32 status, uint64 validtime /* = 0u */ )
    {
        auto kftaskrecord = player->Find( __STRING__( task ) );
        auto kftask = kftaskrecord->Find( setting->_id );
        if ( kftask == nullptr )
        {
            kftask = player->CreateData( kftaskrecord );
            kftask->Set( __STRING__( status ), status );
            SetTaskTime( player, kftask, setting, validtime, false );

            // 完成条件
            auto condition_object = kftask->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, condition_object, setting->_complete_condition );
            InitTaskCondition( player, setting, kftask, status, false );

            player->AddRecord( kftaskrecord, setting->_id, kftask );
        }
        else
        {
            // 更新状态
            UpdataTaskStatus( player, setting, kftask, status, validtime );
        }

        return kftask;
    }

    void KFTaskModule::UpdataTaskStatus( EntityPtr player, const KFTaskSetting* setting, DataPtr kftask, uint32 status, uint64 validtime )
    {
        auto laststatus = kftask->Get<uint32>( __STRING__( status ) );
        if ( laststatus == status )
        {
            return;
        }

        // 更新状态
        player->UpdateObject( kftask, __STRING__( status ), KFEnum::Set, status );

        // 设置时间
        SetTaskTime( player, kftask, setting, validtime, true );

        // 初始化条件
        InitTaskCondition( player, setting, kftask, status, true );
    }

    void KFTaskModule::SetTaskTime( EntityPtr player, DataPtr kftask, const KFTaskSetting* setting, uint64 validtime, bool update )
    {
        if ( validtime == 0u )
        {
            return;
        }

        validtime += KFGlobal::Instance()->_real_time;
        if ( !update )
        {
            kftask->Set( __STRING__( time ), validtime );
        }
        else
        {
            player->UpdateObject( kftask, __STRING__( time ), KFEnum::Set, validtime );
        }

        // 启动定时器
        StartTaskTimeoutTimer( player, setting->_id, validtime );
    }

    void KFTaskModule::AddFinishTask( EntityPtr player, DataPtr kftask, const KFTaskSetting* setting, bool update )
    {
        // 不是自动提交
        if ( setting->_complete_mode != KFTaskEnum::ComplelteAuto )
        {
            return;
        }

        if ( update )
        {
            FinishTask( player, kftask, setting );
        }
        else
        {
            // 新加的任务, 启动一个定时器
            __COUNT_TIMER_2__( player->GetKeyID(), setting->_id, 10u, 1, &KFTaskModule::OnTimerTaskFinish );
        }
    }

    void KFTaskModule::FinishTask( EntityPtr player, uint32 taskid )
    {
        auto kftask = player->Find( __STRING__( task ), taskid );
        if ( kftask == nullptr )
        {
            return;
        }

        auto setting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );
        }

        FinishTask( player, kftask, setting );
    }

    void KFTaskModule::FinishTask( EntityPtr player, DataPtr kftask, const KFTaskSetting* setting )
    {
        kftask->Set<uint32>( __STRING__( type ), setting->_type );
        kftask->Set<uint32>( __STRING__( status ), KFMsg::ReceiveStatus );

        // 任务输出执行
        _kf_execute->Execute( player, setting->_execute, __STRING__( task ), setting->_id, __FUNC_LINE__ );

        // 删除任务
        player->RemoveRecord( kftask->GetParent(), setting->_id );

        // 提示客户端
        _kf_display->DelayToClient( player, KFMsg::TaskRewardOk, setting->_id );
    }

    void KFTaskModule::StartTaskTimeoutTimer( EntityPtr player, uint32 taskid, uint64 timeout )
    {
        auto lefttime = 1u;
        if ( timeout > KFGlobal::Instance()->_real_time )
        {
            lefttime = ( timeout - KFGlobal::Instance()->_real_time ) * 1000u;
        }

        __COUNT_TIMER_2__( player->GetKeyID(), taskid, lefttime, 1, &KFTaskModule::OnTimerTaskTimeout );
    }

    __KF_TIMER_FUNCTION__( KFTaskModule::OnTimerTaskTimeout )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        player->RemoveRecord( __STRING__( task ), subid );
    }

    void KFTaskModule::StopTaskTimeoutTimer( EntityPtr player, DataPtr kftask, uint32 taskid )
    {
        auto time = kftask->Get( __STRING__( time ) );
        if ( time != 0u )
        {
            __UN_TIMER_2__( player->GetKeyID(), taskid );
            player->UpdateObject( kftask, __STRING__( time ), KFEnum::Set, 0u );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskModule::InitTaskCondition( EntityPtr player, const KFTaskSetting* setting, DataPtr kftask, uint32 status, bool update )
    {
        // 不在领取状态, 不初始化任务条件
        if ( status != KFMsg::ExecuteStatus )
        {
            return;
        }

        // 初始化条件
        auto condition_object = kftask->Find( __STRING__( conditions ) );
        auto complete = _kf_condition->InitCondition( player, condition_object, update );
        if ( complete )
        {
            DoneTask( player, kftask, setting, update );
        }

        _kf_display->DelayToClient( player, KFMsg::TaskReceiveOk, setting->_id );
    }

    void KFTaskModule::DoneTask( EntityPtr player, DataPtr kftask, const KFTaskSetting* setting, bool update )
    {
        auto kfstatus = kftask->Find( __STRING__( status ) );
        auto status = kfstatus->Get<uint32>();
        if ( status == KFMsg::DoneStatus || status == KFMsg::ReceiveStatus )
        {
            return;
        }

        // 任务完成, 停止倒计时
        StopTaskTimeoutTimer( player, kftask, setting->_id );
        if ( !update )
        {
            kfstatus->Set<uint32>( KFMsg::DoneStatus );
        }
        else
        {
            player->UpdateData( kfstatus, KFEnum::Set, KFMsg::DoneStatus );
        }

        // 刚创建的任务, 如果是完成状态, 又是自动交付, 延迟删除
        AddFinishTask( player, kftask, setting, update );
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

    class KFUpdateTaskData
    {
    public:
        DataPtr _task;
        const KFTaskSetting* _kfsetting = nullptr;
    };

#define __GET_UPDATA_TASK_LIST__()\
    std::list< KFUpdateTaskData > _update_task;\
    auto kftaskrecord = player->Find( __STRING__( task ) );\
    for ( auto kftask = kftaskrecord->First(); kftask != nullptr; kftask = kftaskrecord->Next() )\
    {\
        auto taskid = kftask->Get<uint32>( __STRING__( id ) );\
        auto setting = KFTaskConfig::Instance()->FindSetting( taskid );\
        if ( setting == nullptr )\
        {\
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );\
            continue;\
        }\
        auto status = kftask->Get<uint32>( __STRING__( status ) );\
        if ( status == KFMsg::ExecuteStatus )\
        {\
            KFUpdateTaskData updatedata; \
            updatedata._task = kftask; \
            updatedata._kfsetting = setting; \
            _update_task.emplace_back( updatedata ); \
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFTaskModule::OnAddDataTaskModule )
    {
        __GET_UPDATA_TASK_LIST__();

        for ( auto& data : _update_task )
        {
            auto kftask = data._task;
            auto setting = data._kfsetting;

            auto condition_object = kftask->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateAddCondition( player, condition_object, kfdata );
            if ( complete )
            {
                DoneTask( player, kftask, setting, true );
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFTaskModule::OnRemoveDataTaskModule )
    {
        __GET_UPDATA_TASK_LIST__();

        for ( auto& data : _update_task )
        {
            auto kftask = data._task;
            auto setting = data._kfsetting;

            auto condition_object = kftask->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateRemoveCondition( player, condition_object, kfdata );
            if ( complete )
            {
                DoneTask( player, kftask, setting, true );
            }
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateDataTaskModule )
    {
        __GET_UPDATA_TASK_LIST__();

        for ( auto& data : _update_task )
        {
            auto kftask = data._task;
            auto setting = data._kfsetting;

            auto condition_object = kftask->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateUpdateCondition( player, condition_object, kfdata, operate, value, newvalue );
            if ( complete )
            {
                DoneTask( player, kftask, setting, true );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskReceiveReq, KFMsg::MsgTaskReceiveReq )
    {
        auto setting = KFTaskConfig::Instance()->FindSetting( kfmsg->id() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskCanNotFind, kfmsg->id() );
        }

        auto kftask = entity->Find( __STRING__( task ), kfmsg->id() );
        if ( kftask == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskNotActive );
        }

        auto status = kftask->Get<uint32>( __STRING__( status ) );
        if ( status != KFMsg::ActiveStatus )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskAlreadyReceive );
        }

        UpdataTaskStatus( entity, setting, kftask, KFMsg::ExecuteStatus, 0u );
    }

    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskRewardReq, KFMsg::MsgTaskRewardReq )
    {
        auto setting = KFTaskConfig::Instance()->FindSetting( kfmsg->id() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskCanNotFind, kfmsg->id() );
        }

        auto kftask = entity->Find( __STRING__( task ), kfmsg->id() );
        if ( kftask == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskCanNotFindData );
        }

        // 不是完成状态
        auto taskstatus = kftask->Get<uint32>( __STRING__( status ) );
        if ( taskstatus != KFMsg::DoneStatus )
        {
            if ( taskstatus == KFMsg::ReceiveStatus )
            {
                return _kf_display->SendToClient( entity, KFMsg::TaskAlreadyReward );
            }
            else
            {
                return _kf_display->SendToClient( entity, KFMsg::TaskNotDone );
            }
        }

        // 交付完成任务
        FinishTask( entity, kftask, setting );
    }

    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskRemoveReq, KFMsg::MsgTaskRemoveReq )
    {
        auto setting = KFTaskConfig::Instance()->FindSetting( kfmsg->id() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskCanNotFind, kfmsg->id() );
        }

        entity->RemoveRecord( __STRING__( task ), kfmsg->id() );
    }

    __KF_EXECUTE_FUNCTION__( KFTaskModule::OnExecuteUpdateTaskStatus )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            return false;
        }

        auto taskid = executedata->_param_list._params[ 0 ]->_int_value;
        auto status = executedata->_param_list._params[ 1 ]->_int_value;
        auto setting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( setting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, taskid );
            return false;
        }

        auto kftaskrecord = player->Find( __STRING__( task ) );
        switch ( status )
        {
        case KFMsg::InitStatus:
        {
            player->RemoveRecord( kftaskrecord, taskid );
        }
        break;
        case KFMsg::ActiveStatus:
        {
            auto kftask = kftaskrecord->Find( taskid );
            if ( kftask != nullptr )
            {
                // 更新状态
                UpdataTaskStatus( player, setting, kftask, status, 0u );
            }
            else
            {
                // 开启任务
                CreateTask( player, setting, status );
            }
        }
        break;
        case KFMsg::ExecuteStatus:
        {
            // 开启任务
            CreateTask( player, setting, status, 0u );
        }
        break;
        case KFMsg::DoneStatus:
        {
            auto kftask = kftaskrecord->Find( taskid );
            if ( kftask != nullptr )
            {
                DoneTask( player, kftask, setting, true );
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
        auto condition_id = executedata->_param_list._params[ 1 ]->_int_value;
        auto operate = executedata->_param_list._params[ 2 ]->_int_value;
        auto condition_value = executedata->_param_list._params[ 3 ]->_int_value;
        auto setting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( setting == nullptr )
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
                auto condition_object = kftask->Find( __STRING__( conditions ) );
                auto ok = _kf_condition->UpdateCondition( player, condition_object, condition_id, operate, condition_value );
                if ( ok )
                {
                    DoneTask( player, kftask, setting, true );
                }
            }
        }

        return true;
    }
}