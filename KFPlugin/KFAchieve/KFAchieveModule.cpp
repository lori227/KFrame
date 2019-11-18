#include "KFAchieveModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFAchieveModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ADD_DATA__( &KFAchieveModule::OnAddDataCallBack );
        __REGISTER_REMOVE_DATA__( &KFAchieveModule::OnRemoveDataCallBack );
        __REGISTER_UPDATE_DATA__( &KFAchieveModule::OnUpdateDataCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( achieve ), __STRING__( value ), &KFAchieveModule::OnAchieveValueUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ACHIEVE_REWARD_REQ, &KFAchieveModule::HandleAchieveRewardReq );
    }

    void KFAchieveModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_REMOVE_DATA__();
        __UN_UPDATE_DATA__();
        __UN_UPDATE_DATA_2__( __STRING__( achieve ), __STRING__( value ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_ACHIEVE_REWARD_REQ );
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
        auto kfsetting = KFAchieveConfig::Instance()->FindSetting( achieveid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::AchieveCanNotFind;
        }

        auto kfachieve = player->Find( __STRING__( achieve ), kfsetting->_id );
        if ( kfachieve == nullptr )
        {
            return KFMsg::AchieveCanNotFindData;
        }

        auto status = kfachieve->Get( __STRING__( status ) );
        if ( status == KFMsg::InitStatus )
        {
            return KFMsg::AchieveNotDone;
        }

        if ( status == KFMsg::ReceiveStatus )
        {
            return KFMsg::AchieveAlreadyReceived;
        }

        // 设置已经领取
        player->UpdateData( kfachieve, __STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );

        // 添加奖励
        player->AddElement( &kfsetting->_rewards, __STRING__( achieve ), __FUNC_LINE__ );
        return KFMsg::AchieveReceiveOk;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFAchieveModule::OnAchieveValueUpdateCallBack )
    {
        auto achievesetting = KFAchieveConfig::Instance()->FindSetting( key );
        if ( achievesetting == nullptr )
        {
            return;
        }

        bool doneachieve = KFUtility::CheckOperate< uint64 >( newvalue, achievesetting->_done_type, achievesetting->_done_value );
        if ( !doneachieve )
        {
            return;
        }

        player->UpdateData( __STRING__( achieve ), key, __STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );
        if ( newvalue > achievesetting->_done_value )
        {
            kfdata->Operate< uint64 >( KFEnum::Set, achievesetting->_done_value );
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

        for ( auto kfchild = kfdata->First(); kfchild != nullptr; kfchild = kfdata->Next() )
        {
            auto value = kfchild->Get();
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

        auto kfachievetype = KFAchieveConfig::Instance()->FindTypeAchieve( kfdata->GetParent()->_data_setting->_name, kfdata->_data_setting->_name );
        if ( kfachievetype == nullptr )
        {
            return;
        }

        auto kfachieves = player->Find( __STRING__( achieve ) );

        for ( auto kfsetting : kfachievetype->_achieve_list )
        {
            // 是否已经完成
            auto flag = kfachieves->Get< uint32 >( kfsetting->_id, __STRING__( status ) );
            if ( flag != KFMsg::InitStatus )
            {
                continue;
            }

            // 判断是否满足条件
            if ( !kfsetting->_limits.IsEmpty() )
            {
                auto& dataname = player->CheckRemoveElement( &kfsetting->_limits, __FUNC_LINE__ );
                if ( !dataname.empty() )
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

            player->UpdateData( kfachieves, kfsetting->_id, __STRING__( value ), kfsetting->_use_operate, operatevalue );
        }
    }
}