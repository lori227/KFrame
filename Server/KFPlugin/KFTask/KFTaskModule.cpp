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
        _kf_component->RegisterAddDataModule( this, &KFTaskModule::OnAddDataCallBack );
        _kf_component->RegisterRemoveDataModule( this, &KFTaskModule::OnRemoveDataCallBack );
        _kf_component->RegisterUpdateDataModule( this, &KFTaskModule::OnUpdateDataCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( task ), __KF_STRING__( value ), this, &KFTaskModule::OnUpdateTaskValueCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( task ), __KF_STRING__( flag ), this, &KFTaskModule::OnUpdateTaskFlagCallBack );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_TASK_REWARD_REQ, &KFTaskModule::HandleReceiveTaskRewardReq );
    }

    void KFTaskModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_component->UnRegisterAddDataModule( this );
        _kf_component->UnRegisterRemoveDataModule( this );
        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( task ), __KF_STRING__( value ) );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( task ), __KF_STRING__( flag ) );
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
        player->UpdateData( kftask, __KF_STRING__( flag ), KFOperateEnum::Set, KFMsg::FlagEnum::Received );

        // 添加奖励
        player->AddAgentData( &tasksetting->_rewards, 1.0f, true, __FUNC_LINE__ );
        return KFMsg::TaskReceiveRewardOK;
    }


    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateDataCallBack )
    {
        if ( value == 0 )
        {
            return;
        }

        UpdateDataTaskValue( player, key, kfdata, operate, value, newvalue );
    }

    __KF_ADD_DATA_FUNCTION__( KFTaskModule::OnAddDataCallBack )
    {
        UpdateObjectTaskValue( player, key, kfdata, KFOperateEnum::Add );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskModule::OnRemoveDataCallBack )
    {
        UpdateObjectTaskValue( player, key, kfdata, KFOperateEnum::Dec );
    }

    void KFTaskModule::UpdateObjectTaskValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate )
    {
        auto kfchild = kfdata->FirstData();
        while ( kfchild != nullptr )
        {
            auto value = kfchild->GetValue();
            if ( value != 0 )
            {
                UpdateDataTaskValue( player, key, kfchild, operate, value, value );
            }

            kfchild = kfdata->NextData();
        }
    }

    void KFTaskModule::UpdateDataTaskValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        auto kftasktypelist = _kf_task_config->FindTypeTaskList( kfdata->GetParent()->GetName(), kfdata->GetName() );
        if ( kftasktypelist == nullptr )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kftaskrecord = kfobject->FindData( __KF_STRING__( task ) );
        auto level = kfobject->GetValue< uint64 >( __KF_STRING__( basic ), __KF_STRING__( level ) );

        for ( auto kfsetting : kftasktypelist->_task_type )
        {
            if ( !kfsetting->CheckCanUpdate( key, level, operate ) )
            {
                continue;
            }

            // 判断触发值
            auto operatevalue = kfsetting->CheckTriggerValue( value, nowvalue );
            if ( operatevalue == 0 )
            {
                continue;
            }

            // 已经完成
            auto taskflag = kftaskrecord->GetValue< uint32 >( kfsetting->_id, __KF_STRING__( flag ) );
            if ( taskflag != KFMsg::FlagEnum::Init )
            {
                continue;
            }

            // 获得使用的数值, 更新任务数值
            auto usevalue = kfsetting->CalcUseValue( operatevalue );
            player->UpdateData( kftaskrecord, kfsetting->_id, __KF_STRING__( value ), kfsetting->_operate, usevalue );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateTaskValueCallBack )
    {
        auto kfsetting = _kf_task_config->FindTaskSetting( static_cast< uint32 >( key ) );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 判断是否满足完成条件
        if ( newvalue < kfsetting->_done_value )
        {
            return;
        }

        auto kfparent = kfdata->GetParent();
        auto taskflag = kfparent->GetValue<uint32>( __KF_STRING__( flag ) );
        if ( taskflag != KFMsg::FlagEnum::Init )
        {
            return;
        }

        // 设置任务完成
        player->UpdateData( kfparent, __KF_STRING__( flag ), KFOperateEnum::Set, KFMsg::FlagEnum::Done );

        // 更新下一个任务
        if ( kfsetting->_next_value != 0 && kfsetting->_next_id != 0 )
        {
            auto taskvalue = kfparent->GetValue< uint32 >( __KF_STRING__( value ) );
            player->UpdateData( __KF_STRING__( task ), kfsetting->_next_id, __KF_STRING__( value ), KFOperateEnum::Set, taskvalue );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateTaskFlagCallBack )
    {
        auto kfsetting = _kf_task_config->FindTaskSetting( static_cast< uint32 >( key ) );
        if ( kfsetting == nullptr )
        {
            return;
        }

        if ( newvalue == KFMsg::FlagEnum::Init )
        {
            // 更新数值
            player->UpdateData( __KF_STRING__( task ), key, __KF_STRING__( value ), KFOperateEnum::Set, 0 );
        }
    }

}