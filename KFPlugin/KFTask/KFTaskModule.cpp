#include "KFTaskModule.hpp"

namespace KFrame
{
    namespace KFTaskEnum
    {
        enum MyEnum
        {
            CompleteRequest = 1,	// 玩家请求交付
            CompleteAuto = 2,		// 自动交付
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskModule::BeforeRun()
    {
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
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
        if ( !element_result->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", element_result->_element->_data_name );
            return false;
        }

        auto element_object = std::dynamic_pointer_cast<KFElementObject>( element_result->_element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", element_result->_element->_data_name );
            return false;
        }

        // 任务状态
        auto status = element_object->CalcValue( element_result->_element->_data_setting, __STRING__( status ) );
        auto chain_id = element_object->CalcValue( element_result->_element->_data_setting, __STRING__( chain ) );
        auto chain_index = element_object->CalcValue( element_result->_element->_data_setting, __STRING__( index ) );
        auto valid_time = element_object->CalcValue( element_result->_element->_data_setting, __STRING__( validtime ) );
        OpenTask( player, element_object->_config_id, status, valid_time, chain_id, chain_index );
        return true;
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskModule::OnRemoveTask )
    {
        // 如果存在定时器
        auto time = data->Get<uint32>( __STRING__( time ) );
        if ( time != 0u )
        {
            __UN_TIMER_2__( player->GetKeyID(), key );
        }
    }

    __KF_PLAYER_ENTER_FUNCTION__( KFTaskModule::OnEnterTaskModule )
    {
        UInt64List remove_list;
        std::map<DataPtr, std::shared_ptr<const KFTaskSetting>> finish_task_list;

        auto task_record = player->Find( __STRING__( task ) );
        for ( auto task_data = task_record->First(); task_data != nullptr; task_data = task_record->Next() )
        {
            auto task_id = task_data->Get<uint32>( __STRING__( id ) );
            auto setting = KFTaskConfig::Instance()->FindSetting( task_id );
            if ( setting == nullptr )
            {
                remove_list.push_back( task_id );
                continue;
            }

            auto status = task_data->Get<uint32>( __STRING__( status ) );
            if ( status == KFMsg::DoneStatus )
            {
                finish_task_list[ task_data ] = setting;
                continue;
            }

            // 超时定时器
            auto time = task_data->Get( __STRING__( time ) );
            if ( time != 0u )
            {
                StartTaskTimeoutTimer( player, task_id, time );
            }
        }

        // 删除配置错误的任务
        for ( auto task_id : remove_list )
        {
            task_record->Remove( task_id );
        }

        // 出来已经完成的任务
        for ( auto& iter : finish_task_list )
        {
            AddFinishTask( player, iter.first, iter.second, true );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFTaskModule::OnLeaveTaskModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    DataPtr KFTaskModule::OpenTask( EntityPtr player, uint32 task_id, uint32 status, uint64 valid_time /* = 0u */,
                                    uint32 chain_id /* = 0u */, uint32 chain_index /* = 1u */ )
    {
        auto setting = KFTaskConfig::Instance()->FindSetting( task_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "task=[{}] can't find setting", task_id );
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, task_id );
            return nullptr;
        }

        auto task_data = CreateTask( player, setting, status, valid_time );
        task_data->Operate( __STRING__( chain ), KFEnum::Set, chain_id );
        task_data->Operate( __STRING__( index ), KFEnum::Set, chain_index );
        return task_data;
    }

    DataPtr KFTaskModule::CreateTask( EntityPtr player, std::shared_ptr<const KFTaskSetting> setting, uint32 status, uint64 valid_time /* = 0u */ )
    {
        auto task_record = player->Find( __STRING__( task ) );
        auto task_data = task_record->Find( setting->_id );
        if ( task_data == nullptr )
        {
            task_data = player->CreateData( task_record );
            task_data->Operate( __STRING__( status ), KFEnum::Set, status );
            SetTaskTime( player, task_data, setting, valid_time, false );

            // 完成条件
            auto condition_object = task_data->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, condition_object, setting->_complete_condition );
            InitTaskCondition( player, setting, task_data, status, false );

            player->AddRecord( task_record, setting->_id, task_data );
        }
        else
        {
            // 更新状态
            UpdateTaskStatus( player, setting, task_data, status, valid_time );
        }

        return task_data;
    }

    void KFTaskModule::UpdateTaskStatus( EntityPtr player, std::shared_ptr<const KFTaskSetting> setting, DataPtr task_data, uint32 status, uint64 valid_time )
    {
        auto last_status = task_data->Get<uint32>( __STRING__( status ) );
        if ( last_status == status )
        {
            return;
        }

        // 更新状态
        player->UpdateObject( task_data, __STRING__( status ), KFEnum::Set, status );

        // 设置时间
        SetTaskTime( player, task_data, setting, valid_time, true );

        // 初始化条件
        InitTaskCondition( player, setting, task_data, status, true );
    }

    void KFTaskModule::SetTaskTime( EntityPtr player, DataPtr task_data, std::shared_ptr<const KFTaskSetting> setting, uint64 valid_time, bool update )
    {
        if ( valid_time == 0u )
        {
            return;
        }

        valid_time += KFGlobal::Instance()->_real_time;
        if ( !update )
        {
            task_data->Operate( __STRING__( time ), KFEnum::Set, valid_time );
        }
        else
        {
            player->UpdateObject( task_data, __STRING__( time ), KFEnum::Set, valid_time );
        }

        // 启动定时器
        StartTaskTimeoutTimer( player, setting->_id, valid_time );
    }

    void KFTaskModule::AddFinishTask( EntityPtr player, DataPtr task_data, std::shared_ptr<const KFTaskSetting> setting, bool update )
    {
        // 不是自动提交
        if ( setting->_complete_mode != KFTaskEnum::CompleteAuto )
        {
            return;
        }

        if ( update )
        {
            FinishTask( player, task_data, setting );
        }
        else
        {
            // 新加的任务, 启动一个定时器
            __COUNT_TIMER_2__( player->GetKeyID(), setting->_id, 10u, 1, &KFTaskModule::OnTimerTaskFinish );
        }
    }

    void KFTaskModule::FinishTask( EntityPtr player, uint32 task_id )
    {
        auto task_data = player->Find( __STRING__( task ), task_id );
        if ( task_data == nullptr )
        {
            return;
        }

        auto setting = KFTaskConfig::Instance()->FindSetting( task_id );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, task_id );
        }

        FinishTask( player, task_data, setting );
    }

    void KFTaskModule::FinishTask( EntityPtr player, DataPtr task_data, std::shared_ptr<const KFTaskSetting> setting )
    {
        task_data->Operate( __STRING__( type ), KFEnum::Set, setting->_type );
        task_data->Operate( __STRING__( status ), KFEnum::Set, (uint32)KFMsg::ReceiveStatus );

        // 任务输出执行
        _kf_execute->Execute( player, setting->_execute, __STRING__( task ), setting->_id, __FUNC_LINE__ );

        // 删除任务
        player->RemoveRecord( task_data->GetParent()->GetDataPtr(), setting->_id );

        // 提示客户端
        _kf_display->DelayToClient( player, KFMsg::TaskRewardOk, setting->_id );
    }

    void KFTaskModule::StartTaskTimeoutTimer( EntityPtr player, uint32 task_id, uint64 timeout )
    {
        auto left_time = 1u;
        if ( timeout > KFGlobal::Instance()->_real_time )
        {
            left_time = ( timeout - KFGlobal::Instance()->_real_time ) * 1000u;
        }

        __COUNT_TIMER_2__( player->GetKeyID(), task_id, left_time, 1, &KFTaskModule::OnTimerTaskTimeout );
    }

    __KF_TIMER_FUNCTION__( KFTaskModule::OnTimerTaskTimeout )
    {
        auto player = _kf_player->FindPlayer( object_id );
        if ( player == nullptr )
        {
            return;
        }

        player->RemoveRecord( __STRING__( task ), data_id );
    }

    void KFTaskModule::StopTaskTimeoutTimer( EntityPtr player, DataPtr task_data, uint32 task_id )
    {
        auto time = task_data->Get( __STRING__( time ) );
        if ( time != 0u )
        {
            __UN_TIMER_2__( player->GetKeyID(), task_id );
            player->UpdateObject( task_data, __STRING__( time ), KFEnum::Set, 0u );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskModule::InitTaskCondition( EntityPtr player, std::shared_ptr<const KFTaskSetting> setting, DataPtr task_data, uint32 status, bool update )
    {
        // 不在领取状态, 不初始化任务条件
        if ( status != KFMsg::ExecuteStatus )
        {
            return;
        }

        // 初始化条件
        auto condition_object = task_data->Find( __STRING__( conditions ) );
        auto complete = _kf_condition->InitCondition( player, condition_object, update );
        if ( complete )
        {
            DoneTask( player, task_data, setting, update );
        }

        _kf_display->DelayToClient( player, KFMsg::TaskReceiveOk, setting->_id );
    }

    void KFTaskModule::DoneTask( EntityPtr player, DataPtr task_data, std::shared_ptr<const KFTaskSetting> setting, bool update )
    {
        auto status_data = task_data->Find( __STRING__( status ) );
        auto status = status_data->Get<uint32>();
        if ( status == KFMsg::DoneStatus || status == KFMsg::ReceiveStatus )
        {
            return;
        }

        // 任务完成, 停止倒计时
        StopTaskTimeoutTimer( player, task_data, setting->_id );
        if ( !update )
        {
            status_data->Set<uint32>( KFMsg::DoneStatus );
        }
        else
        {
            player->UpdateData( status_data, KFEnum::Set, KFMsg::DoneStatus );
        }

        // 刚创建的任务, 如果是完成状态, 又是自动交付, 延迟删除
        AddFinishTask( player, task_data, setting, update );
    }

    __KF_TIMER_FUNCTION__( KFTaskModule::OnTimerTaskFinish )
    {
        auto player = _kf_player->FindPlayer( object_id );
        if ( player == nullptr )
        {
            return;
        }

        FinishTask( player, data_id );
    }

    class KFUpdateTaskData
    {
    public:
        DataPtr _task;
        std::shared_ptr<const KFTaskSetting> _setting = nullptr;
    };

#define __GET_UPDATE_TASK_LIST__()\
    std::list<KFUpdateTaskData> _update_task;\
    auto task_record = player->Find( __STRING__( task ) );\
    for ( auto task_data = task_record->First(); task_data != nullptr; task_data = task_record->Next() )\
    {\
        auto task_id = task_data->Get<uint32>( __STRING__( id ) );\
        auto setting = KFTaskConfig::Instance()->FindSetting( task_id );\
        if ( setting == nullptr )\
        {\
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, task_id );\
            continue;\
        }\
        auto status = task_data->Get<uint32>( __STRING__( status ) );\
        if ( status == KFMsg::ExecuteStatus )\
        {\
            KFUpdateTaskData update_data; \
            update_data._task = task_data; \
            update_data._setting = setting; \
            _update_task.emplace_back( update_data ); \
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFTaskModule::OnAddDataTaskModule )
    {
        __GET_UPDATE_TASK_LIST__();

        for ( auto& update_data : _update_task )
        {
            auto condition_object = update_data._task->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateAddCondition( player, condition_object, data );
            if ( complete )
            {
                DoneTask( player, update_data._task, update_data._setting, true );
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFTaskModule::OnRemoveDataTaskModule )
    {
        __GET_UPDATE_TASK_LIST__();

        for ( auto& update_data : _update_task )
        {
            auto condition_object = update_data._task->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateRemoveCondition( player, condition_object, data );
            if ( complete )
            {
                DoneTask( player, update_data._task, update_data._setting, true );
            }
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateDataTaskModule )
    {
        __GET_UPDATE_TASK_LIST__();

        for ( auto& update_data : _update_task )
        {
            auto condition_object = update_data._task->Find( __STRING__( conditions ) );
            auto complete = _kf_condition->UpdateUpdateCondition( player, condition_object, data, operate, value, new_value );
            if ( complete )
            {
                DoneTask( player, update_data._task, update_data._setting, true );
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

        auto task_data = entity->Find( __STRING__( task ), kfmsg->id() );
        if ( task_data == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskNotActive );
        }

        auto status = task_data->Get<uint32>( __STRING__( status ) );
        if ( status != KFMsg::ActiveStatus )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskAlreadyReceive );
        }

        UpdateTaskStatus( entity, setting, task_data, KFMsg::ExecuteStatus, 0u );
    }

    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskRewardReq, KFMsg::MsgTaskRewardReq )
    {
        auto setting = KFTaskConfig::Instance()->FindSetting( kfmsg->id() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskCanNotFind, kfmsg->id() );
        }

        auto task_data = entity->Find( __STRING__( task ), kfmsg->id() );
        if ( task_data == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TaskCanNotFindData );
        }

        // 不是完成状态
        auto task_status = task_data->Get<uint32>( __STRING__( status ) );
        if ( task_status != KFMsg::DoneStatus )
        {
            if ( task_status == KFMsg::ReceiveStatus )
            {
                return _kf_display->SendToClient( entity, KFMsg::TaskAlreadyReward );
            }
            else
            {
                return _kf_display->SendToClient( entity, KFMsg::TaskNotDone );
            }
        }

        // 交付完成任务
        FinishTask( entity, task_data, setting );
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
        if ( execute_data->_param_list._params.size() < 2u )
        {
            return false;
        }

        auto task_id = execute_data->_param_list._params[ 0 ]->_int_value;
        auto task_status = execute_data->_param_list._params[ 1 ]->_int_value;
        auto setting = KFTaskConfig::Instance()->FindSetting( task_id );
        if ( setting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, task_id );
            return false;
        }

        auto task_record = player->Find( __STRING__( task ) );
        switch ( task_status )
        {
        case KFMsg::InitStatus:
        {
            player->RemoveRecord( task_record, task_id );
        }
        break;
        case KFMsg::ActiveStatus:
        {
            auto task_data = task_record->Find( task_id );
            if ( task_data != nullptr )
            {
                // 更新状态
                UpdateTaskStatus( player, setting, task_data, task_status, 0u );
            }
            else
            {
                // 开启任务
                CreateTask( player, setting, task_status );
            }
        }
        break;
        case KFMsg::ExecuteStatus:
        {
            // 开启任务
            CreateTask( player, setting, task_status, 0u );
        }
        break;
        case KFMsg::DoneStatus:
        {
            auto task_data = task_record->Find( task_id );
            if ( task_data != nullptr )
            {
                DoneTask( player, task_data, setting, true );
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
        if ( execute_data->_param_list._params.size() < 4u )
        {
            return false;
        }

        auto task_id = execute_data->_param_list._params[ 0 ]->_int_value;
        auto condition_id = execute_data->_param_list._params[ 1 ]->_int_value;
        auto operate = execute_data->_param_list._params[ 2 ]->_int_value;
        auto condition_value = execute_data->_param_list._params[ 3 ]->_int_value;
        auto setting = KFTaskConfig::Instance()->FindSetting( task_id );
        if ( setting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::TaskCanNotFind, task_id );
            return false;
        }

        auto task_data = player->Find( __STRING__( task ), task_id );
        if ( task_data != nullptr )
        {
            auto status = task_data->Get<uint32>( __STRING__( status ) );
            if ( status == KFMsg::ExecuteStatus )
            {
                auto condition_object = task_data->Find( __STRING__( conditions ) );
                auto ok = _kf_condition->UpdateCondition( player, condition_object, condition_id, operate, condition_value );
                if ( ok )
                {
                    DoneTask( player, task_data, setting, true );
                }
            }
        }

        return true;
    }
}