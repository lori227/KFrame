#include "KFTaskModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFTaskModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterAddDataModule( this, &KFTaskModule::OnAddDataCallBack );
        _kf_component->RegisterRemoveDataModule( this, &KFTaskModule::OnRemoveDataCallBack );
        _kf_component->RegisterUpdateDataModule( this, &KFTaskModule::OnUpdateDataCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( task ), __KF_STRING__( value ), this, &KFTaskModule::OnUpdateTaskValueCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( task ), __KF_STRING__( status ), this, &KFTaskModule::OnUpdateTaskStatusCallBack );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_TASK_REWARD_REQ, &KFTaskModule::HandleTaskRewardReq );
    }

    void KFTaskModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_component->UnRegisterAddDataModule( this );
        _kf_component->UnRegisterRemoveDataModule( this );
        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( task ), __KF_STRING__( value ) );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( task ), __KF_STRING__( status ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_TASK_REWARD_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTaskModule::HandleTaskRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTaskRewardReq );

        auto result = ReceiveTaskReward( player, kfmsg.id() );
        _kf_display->SendToClient( player, result, kfmsg.id() );
    }

    uint32 KFTaskModule::ReceiveTaskReward( KFEntity* player, uint32 taskid )
    {
        auto kfsetting = KFTaskConfig::Instance()->FindSetting( taskid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::TaskCanNotFind;
        }

        // 获得任务属性
        auto kfobject = player->GetData();
        auto kftask = kfobject->FindData( __KF_STRING__( task ), kfsetting->_id );
        if ( kftask == nullptr )
        {
            return KFMsg::TaskCanNotFindData;
        }

        // 不是完成状态
        auto taskstatus = kftask->GetValue( __KF_STRING__( status ) );
        if ( taskstatus == KFMsg::InitStatus )
        {
            return KFMsg::TaskNotDone;
        }

        // 已经领取
        if ( taskstatus == KFMsg::ReceiveStatus )
        {
            return KFMsg::TaskAlreadyReceived;
        }

        // 更新标记
        player->UpdateData( kftask, __KF_STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );

        // 添加奖励
        player->AddElement( &kfsetting->_rewards, true, __FUNC_LINE__ );
        return KFMsg::TaskRewardOk;
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateDataCallBack )
    {
        if ( value != _invalid_int )
        {
            UpdateDataTaskValue( player, key, kfdata, operate, value, newvalue );
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFTaskModule::OnAddDataCallBack )
    {
        UpdateObjectTaskValue( player, key, kfdata, KFEnum::Add );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTaskModule::OnRemoveDataCallBack )
    {
        UpdateObjectTaskValue( player, key, kfdata, KFEnum::Dec );
    }

    void KFTaskModule::UpdateObjectTaskValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate )
    {
        // 遍历对象所有的值
        for ( auto kfchild = kfdata->FirstData(); kfchild != nullptr; kfchild = kfdata->NextData() )
        {
            auto value = kfchild->GetValue();
            if ( value != _invalid_int )
            {
                UpdateDataTaskValue( player, key, kfchild, operate, value, value );
            }
        }
    }

    void KFTaskModule::UpdateDataTaskValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        auto kftasktypelist = KFTaskConfig::Instance()->FindTypeTaskList( kfdata->GetParent()->_data_setting->_name, kfdata->_data_setting->_name );
        if ( kftasktypelist == nullptr )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kftaskrecord = kfobject->FindData( __KF_STRING__( task ) );

        for ( auto kfsetting : kftasktypelist->_task_list )
        {
            // 已经完成
            auto taskstatus = kftaskrecord->GetValue( kfsetting->_id, __KF_STRING__( status ) );
            if ( taskstatus != KFMsg::InitStatus )
            {
                continue;
            }

            // 如果需要领取任务
            if ( kfsetting->_need_receive )
            {
                auto kftask = kftaskrecord->FindData( kfsetting->_id );
                if ( kftask == nullptr )
                {
                    continue;
                }
            }

            // 限制条件
            if ( kfsetting->_limits.IsEmpty() )
            {
                auto dataname = player->CheckRemoveElement( &kfsetting->_limits, __FUNC_LINE__ );
                if ( !dataname.empty() )
                {
                    continue;
                }
            }

            // 是否能更新
            if ( !kfsetting->CheckCanUpdate( key, operate ) )
            {
                continue;
            }

            // 判断触发值
            auto operatevalue = kfsetting->CheckTriggerValue( value, nowvalue );
            if ( operatevalue == 0u )
            {
                continue;
            }

            player->UpdateData( kftaskrecord, kfsetting->_id, __KF_STRING__( value ), kfsetting->_operate, operatevalue );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateTaskValueCallBack )
    {
        // 判断是否满足完成条件
        auto kfsetting = KFTaskConfig::Instance()->FindSetting( static_cast< uint32 >( key ) );
        if ( kfsetting == nullptr || newvalue < kfsetting->_done_value )
        {
            return;
        }

        auto kfparent = kfdata->GetParent();
        auto taskstatus = kfparent->GetValue( __KF_STRING__( status ) );
        if ( taskstatus != KFMsg::InitStatus )
        {
            return;
        }

        // 设置任务完成
        player->UpdateData( kfparent, __KF_STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );

        // 更新下一个任务
        if ( kfsetting->_next_id != 0u )
        {
            auto taskvalue = 0u;
            if ( kfsetting->_next_value != 0u )
            {
                taskvalue = newvalue;
            }

            player->UpdateData( __KF_STRING__( task ), kfsetting->_next_id, __KF_STRING__( value ), KFEnum::Set, taskvalue );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTaskModule::OnUpdateTaskStatusCallBack )
    {
        auto kfsetting = KFTaskConfig::Instance()->FindSetting( static_cast< uint32 >( key ) );
        if ( kfsetting == nullptr )
        {
            return;
        }

        if ( newvalue == KFMsg::InitStatus )
        {
            // 更新数值
            player->UpdateData( __KF_STRING__( task ), key, __KF_STRING__( value ), KFEnum::Set, 0 );
        }
    }

}