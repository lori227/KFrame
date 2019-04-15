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
        auto kfsetting = _kf_achieve_config->FindAchieveSetting( achieveid );
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

        auto achievestatus = kfachieve->GetValue( __KF_STRING__( status ) );
        if ( achievestatus == KFMsg::InitStatus )
        {
            return KFMsg::AchieveNotDone;
        }

        if ( achievestatus == KFMsg::ReceiveStatus )
        {
            return KFMsg::AchieveAlreadyReceived;
        }

        // 设置已经领取
        player->UpdateData( kfachieve, __KF_STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );

        // 添加奖励
        player->AddElement( __FUNC_LINE__, &kfsetting->_rewards, true );

        return KFMsg::AchieveReceiveOk;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFAchieveModule::OnAchieveValueUpdateCallBack )
    {
        auto achievesetting = _kf_achieve_config->FindAchieveSetting( key );
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
        auto child = kfdata->FirstData();
        while ( child != nullptr )
        {
            auto value = child->GetValue();
            if ( value != 0 )
            {
                UpdateDataAchieveValue( player, key, child, operate, value, value );
            }

            child = kfdata->NextData();
        }
    }

    void KFAchieveModule::UpdateDataAchieveValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue )
    {
        if ( value == 0 )
        {
            return;
        }

        auto& parentname = kfdata->GetParent()->GetName();
        auto& dataname = kfdata->GetName();

        auto kfachievetypesetting = _kf_achieve_config->FindTypeAchieve( parentname, dataname );
        if ( kfachievetypesetting == nullptr )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfachieves = kfobject->FindData( __KF_STRING__( achieve ) );
        if ( kfachieves == nullptr )
        {
            return;
        }

        auto level = kfobject->GetValue( __KF_STRING__( level ) );
        for ( auto achievesetting : kfachievetypesetting->_achieve_type )
        {
            if ( !achievesetting->CheckCanUpdate( key, level, operate ) )
            {
                continue;
            }

            // 判断触发值
            auto operatevalue = achievesetting->CheckTriggerValue( value, nowvalue );
            if ( operatevalue == 0 )
            {
                continue;
            }

            auto kfachieve = kfachieves->FindData( achievesetting->_id );
            if ( kfachieve != nullptr )
            {
                auto flag = kfachieve->GetValue( __KF_STRING__( status ) );
                if ( flag != KFMsg::InitStatus )
                {
                    continue;
                }
            }

            // 获得使用的数值
            auto usevalue = achievesetting->GetUseValue( operatevalue );
            player->UpdateData( kfachieves, achievesetting->_id, __KF_STRING__( value ), achievesetting->_operate, usevalue );
        }
    }
}