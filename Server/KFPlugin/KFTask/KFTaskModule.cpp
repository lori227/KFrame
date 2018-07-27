#include "KFTaskModule.h"
#include "KFTaskConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFTaskModule::KFTaskModule()
    {
        _kf_component = nullptr;
    }

    KFTaskModule::~KFTaskModule()
    {

    }

    void KFTaskModule::InitMoudle()
    {
        __KF_ADD_CONFIG__( _kf_task_config, true );
    }

    void KFTaskModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( task ), __KF_STRING__( value ), this, &KFTaskModule::OnUpdateTaskValueCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( task ), __KF_STRING__( flag ), this, &KFTaskModule::OnUpdateTaskFlagCallBack );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_TASK_REWARD_REQ, &KFTaskModule::HandleReceiveTaskRewardReq );
    }

    void KFTaskModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_component->UnRegisterUpdateDataFunction( __KF_STRING__( task ), __KF_STRING__( value ) );
        _kf_component->UnRegisterUpdateDataFunction( __KF_STRING__( task ), __KF_STRING__( flag ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_TASK_REWARD_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleReceiveTaskRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReceiveTaskRewardReq );

        auto result = ReceiveTaskReward( player, kfmsg.taskid() );
        _kf_display->SendToClient( player, result, kfmsg.taskid() );
    }

    uint32 KFTaskModule::ReceiveTaskReward( KFEntity* player, uint32 taskid )
    {
        auto tasksetting = _kf_task_config->FindTaskSetting( taskid );
        if ( tasksetting == nullptr )
        {
            return KFMsg::TaskIdCanNotFind;
        }

        // 判断是否在时间范围内
        if ( tasksetting->_start_hour != 0 )
        {
            if ( !KFDate::CheckPassTime( 0, 0, 0, tasksetting->_start_hour, tasksetting->_start_minute ) )
            {
                return KFMsg::TaskNotInValidTime;
            }
        }

        if ( tasksetting->_end_hour != 0 )
        {
            if ( KFDate::CheckPassTime( 0, 0, 0, tasksetting->_end_hour, tasksetting->_end_minute ) )
            {
                return KFMsg::TaskNotInValidTime;
            }
        }

        // 获得任务属性
        auto kfobject = player->GetData();
        auto kftask = kfobject->FindData( __KF_STRING__( task ), tasksetting->_id );
        if ( kftask == nullptr )
        {
            return KFMsg::CanNotFindTaskData;
        }

        // 不是完成状态
        auto taskflag = kftask->GetValue<uint32>( __KF_STRING__( flag ) );
        if ( taskflag == KFMsg::FlagEnum::Init )
        {
            return KFMsg::TaskNotDone;
        }

        if ( taskflag == KFMsg::FlagEnum::Received )
        {
            return KFMsg::TaskAlreadyReceived;
        }

        // 更新标记
        player->UpdateData( __KF_STRING__( task ), tasksetting->_id, __KF_STRING__( flag ), KFOperateEnum::Set, KFMsg::FlagEnum::Received );

        // 添加奖励
        player->AddAgentData( &tasksetting->_rewards, 1.0f, true, __FUNCTION_LINE__ );

        return KFMsg::TaskReceiveRewardOK;
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateTaskValueCallBack )
    {
        auto tasksetting = _kf_task_config->FindTaskSetting( static_cast< uint32 >( key ) );
        if ( tasksetting == nullptr )
        {
            return;
        }

        // 判断是否满足完成条件
        auto nowvalue = static_cast< uint32 >( newvalue );
        if ( nowvalue < tasksetting->_done_value )
        {
            return;
        }

        // 没有下一个任务 或者是不继承数值
        if ( tasksetting->_next_id == 0 || tasksetting->_next_value == 0 )
        {
            if ( nowvalue > tasksetting->_done_value )
            {
                kfdata->OperateValue< uint64 >( KFOperateEnum::Set, tasksetting->_done_value );
            }
        }

        auto kfparent = kfdata->GetParent();
        auto flag = kfparent->GetValue<uint32>( __KF_STRING__( flag ) );
        if ( flag != KFMsg::FlagEnum::Init )
        {
            return;
        }

        // 设置任务完成
        player->UpdateData( kfparent->GetName(), key, __KF_STRING__( flag ), KFOperateEnum::Set, KFMsg::FlagEnum::Done );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateTaskFlagCallBack )
    {
        auto tasksetting = _kf_task_config->FindTaskSetting( static_cast< uint32 >( key ) );
        if ( tasksetting == nullptr )
        {
            return;
        }

        switch ( newvalue )
        {
        case KFMsg::FlagEnum::Init:	// 初始化
        {
            auto initvalue = tasksetting->_init_value;
            if ( tasksetting->_done_type == __KF_STRING__( item ) )
            {

            }

            //// 更新数值
            //OperateUInt32( parent, id, FieldDefine::_value, OperateEnum::Set, initvalue );
            break;
        }
        case KFMsg::FlagEnum::Received:	// 已领取
        {
            // 更新下一个任务
            if ( tasksetting->_next_value != 0 && tasksetting->_next_id != 0 )
            {
                auto kfparent = kfdata->GetParent();
                player->UpdateData( kfparent->GetName(), tasksetting->_next_id, __KF_STRING__( value ), KFOperateEnum::Set, newvalue );
            }
            break;
        }
        default:
            break;
        }
    }


}