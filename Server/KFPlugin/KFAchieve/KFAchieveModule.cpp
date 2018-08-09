#include "KFAchieveModule.h"
#include "KFAchieveConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFAchieveModule::KFAchieveModule()
    {
        _kf_component = nullptr;
    }

    KFAchieveModule::~KFAchieveModule()
    {

    }
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
        __REGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_ACHIEVE_REWARD_REQ, &KFAchieveModule::HandleReceiveAchieveRewardReq );
    }

    void KFAchieveModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();

        _kf_component->UnRegisterAddDataModule( this );
        _kf_component->UnRegisterRemoveDataModule( this );
        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( achieve ), __KF_STRING__( value ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_ACHIEVE_REWARD_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAchieveModule::HandleReceiveAchieveRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReceiveAchieveRewardReq );

        auto result = ReceiveAchieveReward( player, kfmsg.achieveid() );
        _kf_display->SendToClient( player, result, kfmsg.achieveid() );
    }

    uint32 KFAchieveModule::ReceiveAchieveReward( KFEntity* player, uint32 achieveid )
    {
        auto kfsetting = _kf_achieve_config->FindAchieveSetting( achieveid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::AchieveIdCanNotFind;
        }

        auto kfobject = player->GetData();
        auto kfachieve = kfobject->FindData( __KF_STRING__( achieve ), kfsetting->_id );
        if ( kfachieve == nullptr )
        {
            return KFMsg::CanNotFindAchieveData;
        }

        auto achieveflag = kfachieve->GetValue( __KF_STRING__( flag ) );
        if ( achieveflag == KFMsg::FlagEnum::Init )
        {
            return KFMsg::AchieveNotDone;
        }

        if ( achieveflag == KFMsg::FlagEnum::Received )
        {
            return KFMsg::AchieveAlreadyReceived;
        }

        // 设置已经领取
        player->UpdateData( kfachieve, __KF_STRING__( flag ), KFOperateEnum::Set, KFMsg::FlagEnum::Received );

        // 添加奖励
        player->AddAgentData( &kfsetting->_rewards, 1.0f, true, __FUNC_LINE__ );

        return KFMsg::AchieveReceiveRewardOK;
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

        bool doneachieve = KFUtility::CheckOperate<uint64>( newvalue, achievesetting->_done_type, achievesetting->_done_value );
        if ( !doneachieve )
        {
            return;
        }

        player->UpdateData( __KF_STRING__( achieve ), key, __KF_STRING__( flag ), KFOperateEnum::Set, KFMsg::FlagEnum::Done );
        if ( newvalue > achievesetting->_done_value )
        {
            kfdata->OperateValue< uint64 >( KFOperateEnum::Set, achievesetting->_done_value );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFAchieveModule::OnUpdateDataCallBack )
    {
        UpdateDataAchieveValue( player, key, kfdata, operate, value, newvalue );
    }

    __KF_ADD_DATA_FUNCTION__( KFAchieveModule::OnAddDataCallBack )
    {
        UpdateObjectAchieveValue( player, key, kfdata, KFOperateEnum::Add );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFAchieveModule::OnRemoveDataCallBack )
    {
        UpdateObjectAchieveValue( player, key, kfdata, KFOperateEnum::Dec );
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

        auto level = kfobject->GetValue< uint64 >( __KF_STRING__( level ) );
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
                auto flag = kfachieve->GetValue( __KF_STRING__( flag ) );
                if ( flag != KFMsg::FlagEnum::Init )
                {
                    continue;
                }
            }

            // 获得使用的数值
            auto usevalue = achievesetting->GetUseValue( operatevalue );
            player->UpdateData( kfachieves, achievesetting->_id, __KF_STRING__( value ), achievesetting->_operate, usevalue );
            // player->UpdateData( kfachieves, achievesetting->_id, __KF_STRING__( type ), KFOperateEnum::Set, KFAchieveEnum::lobby );

        }
    }

    void KFAchieveModule::FormatBattleAchieve( KFData* kfobject, KFMsg::PBTaskDatas* pbachieve )
    {
        auto battlecfg  = _kf_achieve_config->GetBattleAchieveCfg();
        if ( battlecfg.empty() )
        {
            return;
        }

        for ( auto iter : battlecfg )
        {
            auto kfachieve = kfobject->FindData( iter.first );
            if ( nullptr == kfachieve )
            {
                auto achievedata =  pbachieve->add_taskdata();
                achievedata->set_id( iter.first );
                achievedata->set_value( _invalid_int );
            }

            else
            {
                if ( KFMsg::FlagEnum::Init != kfachieve->GetValue<uint32 >( __KF_STRING__( flag ) ) )
                {
                    continue;
                }

                else
                {
                    auto achievedata = pbachieve->add_taskdata();
                    achievedata->set_id( iter.first );
                    achievedata->set_value( kfachieve->GetValue<uint32 >( __KF_STRING__( value ) ) );
                }
            }

        }

    }
}