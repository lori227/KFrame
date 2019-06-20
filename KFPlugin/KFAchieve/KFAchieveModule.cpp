#include "KFAchieveModule.hpp"
#include "KFAchieveConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFAchieveModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_achieve_config, true );
    }

    void KFAchieveModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );

        _kf_component->RegisterAddDataModule( this, &KFAchieveModule::OnAddDataCallBack );
        _kf_component->RegisterRemoveDataModule( this, &KFAchieveModule::OnRemoveDataCallBack );
        _kf_component->RegisterUpdateDataModule( this, &KFAchieveModule::OnUpdateDataCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( achieve ), __KF_STRING__( value ), this, &KFAchieveModule::OnAchieveValueUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ACHIEVE_REWARD_REQ, &KFAchieveModule::HandleAchieveRewardReq );
    }

    void KFAchieveModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_achieve_config );

        _kf_component->UnRegisterAddDataModule( this );
        _kf_component->UnRegisterRemoveDataModule( this );
        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( achieve ), __KF_STRING__( value ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_ACHIEVE_REWARD_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAchieveModule::HandleAchieveRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgAchieveRewardReq );

        auto result = ReceiveAchieveReward( player, kfmsg.id() );
        _kf_display->SendToClient( player, result, kfmsg.id() );
    }

    uint32 KFAchieveModule::ReceiveAchieveReward( KFEntity* player, uint32 achieveid )
    {
        auto kfsetting = _kf_achieve_config->FindSetting( achieveid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::AchieveCanNotFind;
        }

        auto kfobject = player->GetData();
        auto kfachieve = kfobject->FindData( __KF_STRING__( achieve ), kfsetting->_id );
        if ( kfachieve == nullptr )
        {
            return KFMsg::AchieveCanNotFindData;
        }

        auto status = kfachieve->GetValue( __KF_STRING__( status ) );
        if ( status == KFMsg::InitStatus )
        {
            return KFMsg::AchieveNotDone;
        }

        if ( status == KFMsg::ReceiveStatus )
        {
            return KFMsg::AchieveAlreadyReceived;
        }

        // 设置已经领取
        player->UpdateData( kfachieve, __KF_STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );

        // 添加奖励
        player->AddElement( &kfsetting->_rewards, true, __FUNC_LINE__ );
        return KFMsg::AchieveReceiveOk;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFAchieveModule::OnAchieveValueUpdateCallBack )
    {
        auto achievesetting = _kf_achieve_config->FindSetting( key );
        if ( achievesetting == nullptr )
        {
            return;
        }

        bool doneachieve = KFUtility::CheckOperate< uint64 >( newvalue, achievesetting->_done_type, achievesetting->_done_value );
        if ( !doneachieve )
        {
            return;
        }

        player->UpdateData( __KF_STRING__( achieve ), key, __KF_STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );
        if ( newvalue > achievesetting->_done_value )
        {
            kfdata->OperateValue< uint64 >( KFEnum::Set, achievesetting->_done_value );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFAchieveModule::OnUpdateDataCallBack )
    {
        UpdateDataAchieveValue( player, key, kfdata, operate, value, newvalue );
    }

    __KF_ADD_DATA_FUNCTION__( KFAchieveModule::OnAddDataCallBack )
    {
        UpdateObjectAchieveValue( player, key, kfdata, KFEnum::Add );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFAchieveModule::OnRemoveDataCallBack )
    {
        UpdateObjectAchieveValue( player, key, kfdata, KFEnum::Dec );
    }

    void KFAchieveModule::UpdateObjectAchieveValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate )
    {

        for ( auto kfchild = kfdata->FirstData(); kfchild != nullptr; kfchild = kfdata->NextData() )
        {
            auto value = kfchild->GetValue();
            if ( value != 0u )
            {
                UpdateDataAchieveValue( player, key, kfchild, operate, value, value );
            }
        }
    }

    void KFAchieveModule::UpdateDataAchieveValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        if ( value == 0u )
        {
            return;
        }

        auto kfachievetype = _kf_achieve_config->FindTypeAchieve( kfdata->GetParent()->GetName(), kfdata->GetName() );
        if ( kfachievetype == nullptr )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfachieves = kfobject->FindData( __KF_STRING__( achieve ) );

        for ( auto kfsetting : kfachievetype->_achieve_list )
        {
            // 是否已经完成
            auto flag = kfachieves->GetValue< uint32 >( kfsetting->_id, __KF_STRING__( status ) );
            if ( flag != KFMsg::InitStatus )
            {
                continue;
            }

            // 判断是否满足条件
            if ( kfsetting->_limits.IsEmpty() )
            {
                if ( player->CheckElement( &kfsetting->_limits, __FUNC_LINE__ ) )
                {
                    continue;
                }
            }

            // 判断key和操作operate
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

            player->UpdateData( kfachieves, kfsetting->_id, __KF_STRING__( value ), kfsetting->_operate, operatevalue );
        }
    }
}