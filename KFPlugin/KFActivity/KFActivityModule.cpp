#include "KFActivityModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{

    void KFActivityModule::BeforeRun()
    {
        _kf_componnet = _kf_kernel->FindComponent( __STRING__( player ) );

        _kf_componnet->RegisterUpdateDataModule( this, &KFActivityModule::OnUpdateDataCallBack );
        _kf_componnet->RegisterAddDataModule( this, &KFActivityModule::OnAddDataCallBack );
        _kf_componnet->RegisterRemoveDataModule( this, &KFActivityModule::OnRemoveDataCallBack );

        _kf_componnet->RegisterUpdateDataFunction( __STRING__( activity ), __STRING__( value ), this, &KFActivityModule::OnActivityValueUpdateCallBack );

        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ACTIVITY_REWARD_REQ, &KFActivityModule::HandleActivityRewardReq );
    }

    void KFActivityModule::BeforeShut()
    {
        _kf_componnet->UnRegisterAddDataModule( this );
        _kf_componnet->UnRegisterRemoveDataModule( this );
        _kf_componnet->UnRegisterUpdateDataModule( this );
        _kf_componnet->UnRegisterUpdateDataFunction( this, __STRING__( activity ), __STRING__( value ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_ACTIVITY_REWARD_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFActivityModule::HandleActivityRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgActivityRewardReq );

        auto result = ReceiveActivityReward( player, kfmsg.type(), kfmsg.id() );
        _kf_display->SendToClient( player, result );
    }

    uint32 KFActivityModule::ReceiveActivityReward( KFEntity* player, uint32 type, uint32 activityid )
    {
        auto kfsetting = KFActivityConfig::Instance()->FindActivityData( type, activityid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::ActivityCanNotFind;
        }

        auto kfactivity = player->Find( __STRING__( activity ), type );
        if ( kfactivity == nullptr )
        {
            return KFMsg::ActivityCanNotFindData;
        }

        auto received = kfactivity->Get( __STRING__( received ) );
        if ( KFUtility::HaveBitMask<uint64>( received, activityid ) )
        {
            return KFMsg::ActivityAlreadyReceived;
        }

        auto value = kfactivity->Get( __STRING__( value ) );
        if ( value < kfsetting->_done_value )
        {
            return KFMsg::ActivityNotDone;
        }

        // 更新标记
        player->UpdateData( kfactivity, __STRING__( received ), KFEnum::ABit, activityid );

        // 添加奖励
        player->AddElement( &kfsetting->_rewards, true, __FUNC_LINE__ );
        return KFMsg::ActivityRewardOk;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFActivityModule::OnActivityValueUpdateCallBack )
    {
        auto activitysetting = KFActivityConfig::Instance()->FindActivitySetting( key );
        if ( activitysetting == nullptr )
        {
            return;
        }

        auto nowvalue = kfdata->Get();
        if ( nowvalue <= activitysetting->_max_value )
        {
            return;
        }

        kfdata->Operate<uint64>( KFEnum::Set, activitysetting->_max_value );
    }

    void KFActivityModule::UpdateDataActivityValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 newvalue )
    {
        if ( value == _invalid_int )
        {
            return;
        }

        auto kfparent = kfdata->GetParent();
        auto kfactivitytypesetting = KFActivityConfig::Instance()->FindActivityType( kfparent->_data_setting->_name, kfdata->_data_setting->_name );
        if ( kfactivitytypesetting == nullptr )
        {
            return;
        }

        auto kfactivityrecord = player->Find( __STRING__( activity ) );
        if ( kfactivityrecord == nullptr )
        {
            return;
        }

        auto level = player->Get( __STRING__( level ) );
        auto birthday = player->Get( __STRING__( birthday ) );
        auto realtime = KFGlobal::Instance()->_real_time;

        for ( auto activitysetting : kfactivitytypesetting->_activity_type )
        {
            // 判断活动时间
            if ( !activitysetting->CheckInTime( realtime, birthday ) )
            {
                return;
            }

            // 是否能更新
            if ( !activitysetting->CheckCanUpdate( key, level, operate ) )
            {
                continue;
            }

            // 检查出发值
            auto operatevalue = activitysetting->CheckTriggerValue( value, newvalue );
            if ( operatevalue == _invalid_int )
            {
                continue;
            }

            // 检查是否需要重置
            auto kfactivity = kfactivityrecord->Find( activitysetting->_type );
            if ( kfactivity != nullptr )
            {
                auto activitytime = kfactivity->Get( __STRING__( time ) );
                if ( activitysetting->CheckResetActivity( realtime, birthday, activitytime ) )
                {
                    player->UpdateData( kfactivityrecord, activitysetting->_type, __STRING__( value ), KFEnum::Set, _invalid_int );
                    player->UpdateData( kfactivityrecord, activitysetting->_type, __STRING__( received ), KFEnum::Set, _invalid_int );
                    player->UpdateData( kfactivityrecord, activitysetting->_type, __STRING__( time ), KFEnum::Set, realtime );
                }

                auto donevalue = kfactivity->Get( __STRING__( value ) );
                if ( donevalue >= activitysetting->_max_value )
                {
                    continue;
                }
            }

            auto usevalue = activitysetting->GetUseValue( operatevalue );
            player->UpdateData( kfactivityrecord, activitysetting->_type, __STRING__( value ), activitysetting->_operate, usevalue );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFActivityModule::OnUpdateDataCallBack )
    {
        UpdateDataActivityValue( player, key, kfdata, operate, value, newvalue );
    }

    void KFActivityModule::UpdateObjectActivityValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate )
    {
        auto child = kfdata->First();
        while ( child != nullptr )
        {
            auto value = child->Get();
            if ( value != _invalid_int )
            {
                UpdateDataActivityValue( player, key, child, operate, value, value );
            }

            child = kfdata->Next();
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFActivityModule::OnAddDataCallBack )
    {
        UpdateObjectActivityValue( player, key, kfdata, KFEnum::Add );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFActivityModule::OnRemoveDataCallBack )
    {
        UpdateObjectActivityValue( player, key, kfdata, KFEnum::Dec );
    }

    void KFActivityModule::CheckResetDailyActivity( KFEntity* player )
    {
        auto kfactivityrecord = player->Find( __STRING__( activity ) );
        if ( kfactivityrecord == nullptr )
        {
            return;
        }

        std::list< uint64 > removes;

        auto birthday = player->Get( __STRING__( birthday ) );
        auto realtime = KFGlobal::Instance()->_real_time;

        auto kfactivity = kfactivityrecord->First();
        while ( kfactivity != nullptr )
        {
            auto type = kfactivity->GetKeyID();
            auto activitytime = kfactivity->Get( __STRING__( time ) );

            auto activitysetting = KFActivityConfig::Instance()->FindActivitySetting( type );
            if ( activitysetting == nullptr ||
                    !activitysetting->CheckInTime( realtime, birthday ) ||
                    activitysetting->CheckResetActivity( realtime, birthday, activitytime ) )
            {
                removes.push_back( type );
            }

            kfactivity = kfactivityrecord->Next();
        }

        for ( auto type : removes )
        {
            player->RemoveData( kfactivityrecord, type );
        }
    }
}