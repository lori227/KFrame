#include "KFAchieveModule.hpp"

namespace KFrame
{
    void KFAchieveModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ADD_DATA__( &KFAchieveModule::OnAddDataAchieveModule );
        __REGISTER_REMOVE_DATA__( &KFAchieveModule::OnRemoveDataAchieveModule );
        __REGISTER_UPDATE_DATA__( &KFAchieveModule::OnUpdateDataAchieveModule );

        __REGISTER_PLAYER_ENTER__( &KFAchieveModule::OnEnterAchieveModule );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_ACHIEVE_REWARD_REQ, &KFAchieveModule::HandleAchieveRewardReq );
    }

    void KFAchieveModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_REMOVE_DATA__();
        __UN_UPDATE_DATA__();

        __UN_PLAYER_ENTER__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_ACHIEVE_REWARD_REQ );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFAchieveModule::OnEnterAchieveModule )
    {
        auto kfachieverecord = player->Find( __STRING__( achieve ) );
        for ( auto& iter : KFAchieveConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            auto kfachieve = kfachieverecord->Find( setting->_id );
            if ( kfachieve != nullptr )
            {
                continue;
            }

            kfachieve = player->CreateData( kfachieverecord );
            auto kfconditionobject = kfachieve->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, kfconditionobject, setting->_complete_condition );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, kfconditionobject, false );
            if ( complete )
            {
                kfachieve->Set<uint32>( __STRING__( status ), KFMsg::DoneStatus );
            }

            kfachieverecord->Add( setting->_id, kfachieve );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAchieveModule::HandleAchieveRewardReq, KFMsg::MsgAchieveRewardReq )
    {
        auto result = ReceiveAchieveReward( kfentity, kfmsg->id() );
        _kf_display->SendToClient( kfentity, result, kfmsg->id() );
    }

    uint32 KFAchieveModule::ReceiveAchieveReward( EntityPtr player, uint32 achieveid )
    {
        auto setting = KFAchieveConfig::Instance()->FindSetting( achieveid );
        if ( setting == nullptr )
        {
            return KFMsg::AchieveCanNotFind;
        }

        auto kfachieve = player->Find( __STRING__( achieve ), setting->_id );
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
        player->UpdateObject( kfachieve, __STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );

        // 添加奖励
        player->AddElement( &setting->_reward, _default_multiple, __STRING__( achieve ), setting->_id, __FUNC_LINE__ );
        return KFMsg::AchieveReceiveOk;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __UPDATA_ACHIEVE_LIST__( updatefunction )\
    std::set< DataPtr > _update_list;\
    auto kfachieverecord = player->Find( __STRING__( achieve ) );\
    for ( auto kfachieve = kfachieverecord->First(); kfachieve != nullptr; kfachieve = kfachieverecord->Next() )\
    {\
        auto achieveid = kfachieve->Get<uint32>( __STRING__( id ) );\
        auto setting = KFAchieveConfig::Instance()->FindSetting( achieveid );\
        if ( setting == nullptr )\
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
            player->UpdateObject( kfachieve, __STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFAchieveModule::OnAddDataAchieveModule )
    {
        __UPDATA_ACHIEVE_LIST__( _kf_condition->UpdateAddCondition( player, kfconditionobject, kfdata ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFAchieveModule::OnRemoveDataAchieveModule )
    {
        __UPDATA_ACHIEVE_LIST__( _kf_condition->UpdateRemoveCondition( player, kfconditionobject, kfdata ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFAchieveModule::OnUpdateDataAchieveModule )
    {
        __UPDATA_ACHIEVE_LIST__( _kf_condition->UpdateUpdateCondition( player, kfconditionobject, kfdata, operate, oldvalue, newvalue ) );
    }
}