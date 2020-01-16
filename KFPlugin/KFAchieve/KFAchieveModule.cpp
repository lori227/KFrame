#include "KFAchieveModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFAchieveModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ADD_DATA__( &KFAchieveModule::OnAddDataAchieveModule );
        __REGISTER_REMOVE_DATA__( &KFAchieveModule::OnRemoveDataAchieveModule );
        __REGISTER_UPDATE_DATA__( &KFAchieveModule::OnUpdateDataAchieveModule );

        __REGISTER_ENTER_PLAYER__( &KFAchieveModule::OnEnterAchieveModule );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ACHIEVE_REWARD_REQ, &KFAchieveModule::HandleAchieveRewardReq );
    }

    void KFAchieveModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_REMOVE_DATA__();
        __UN_UPDATE_DATA__();

        __UN_ENTER_PLAYER__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_ACHIEVE_REWARD_REQ );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFAchieveModule::OnEnterAchieveModule )
    {
        auto kfachieverecord = player->Find( __STRING__( achieve ) );
        for ( auto& iter : KFAchieveConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto kfachieve = kfachieverecord->Find( kfsetting->_id );
            if ( kfachieve != nullptr )
            {
                continue;
            }

            kfachieve = player->CreateData( kfachieverecord );
            auto kfconditionobject = kfachieve->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, kfconditionobject, kfsetting->_complete_condition, kfsetting->_complete_condition_type );
            kfachieverecord->Add( kfsetting->_id, kfachieve );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, kfconditionobject, KFConditionEnum::LimitNull, false );
            if ( complete )
            {
                kfachieve->Set<uint32>( __STRING__( status ), KFMsg::DoneStatus );
            }
        }
    }

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
        player->AddElement( &kfsetting->_rewards, _default_multiple, __STRING__( achieve ), kfsetting->_id, __FUNC_LINE__ );
        return KFMsg::AchieveReceiveOk;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __UPDATA_ACHIEVE_LIST__( updatefunction )\
    std::set< KFData* > _update_list;\
    auto kfachieverecord = player->Find( __STRING__( achieve ) );\
    for ( auto kfachieve = kfachieverecord->First(); kfachieve != nullptr; kfachieve = kfachieverecord->Next() )\
    {\
        auto achieveid = kfachieve->Get<uint32>( __STRING__( id ) );\
        auto kfsetting = KFAchieveConfig::Instance()->FindSetting( achieveid );\
        if ( kfsetting == nullptr )\
        {\
            continue;\
        }\
        auto status = kfachieve->Get<uint32>( __STRING__( status ) );\
        if ( status != KFMsg::InitStatus )\
        {\
            continue;\
        }\
        _update_list.insert( kfachieve );\
    }\
    for ( auto kfachieve : _update_list )\
    {\
        auto kfconditionobject = kfachieve->Find( __STRING__( conditions ) );\
        auto complete = updatefunction;\
        if ( complete )\
        {\
            player->UpdateData( kfachieve, __STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFAchieveModule::OnAddDataAchieveModule )
    {
        __UPDATA_ACHIEVE_LIST__( _kf_condition->UpdateAddCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFAchieveModule::OnRemoveDataAchieveModule )
    {
        __UPDATA_ACHIEVE_LIST__( _kf_condition->UpdateRemoveCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFAchieveModule::OnUpdateDataAchieveModule )
    {
        __UPDATA_ACHIEVE_LIST__( _kf_condition->UpdateUpdateCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata, operate, oldvalue, newvalue ) );
    }
}