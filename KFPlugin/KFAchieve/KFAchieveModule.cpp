#include "KFAchieveModule.hpp"

namespace KFrame
{
    void KFAchieveModule::BeforeRun()
    {
        _component = _kf_kernel->FindComponent( __STRING__( player ) );

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
        auto achieve_record = player->Find( __STRING__( achieve ) );
        for ( auto& iter : KFAchieveConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            auto achieve_data = achieve_record->Find( setting->_id );
            if ( achieve_data != nullptr )
            {
                continue;
            }

            achieve_data = player->CreateData( achieve_record );
            auto condition_object = achieve_data->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, condition_object, setting->_complete_condition );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, condition_object, false );
            if ( complete )
            {
                achieve_data->Operate<uint32>( __STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );
            }

            achieve_record->Add( setting->_id, achieve_data );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAchieveModule::HandleAchieveRewardReq, KFMsg::MsgAchieveRewardReq )
    {
        auto result = ReceiveAchieveReward( player, kfmsg->id() );
        _kf_display->SendToClient( player, result, kfmsg->id() );
    }

    uint32 KFAchieveModule::ReceiveAchieveReward( EntityPtr player, uint32 achieve_id )
    {
        auto setting = KFAchieveConfig::Instance()->FindSetting( achieve_id );
        if ( setting == nullptr )
        {
            return KFMsg::AchieveCanNotFind;
        }

        auto achieve_data = player->Find( __STRING__( achieve ), setting->_id );
        if ( achieve_data == nullptr )
        {
            return KFMsg::AchieveCanNotFindData;
        }

        auto status = achieve_data->Get( __STRING__( status ) );
        if ( status == KFMsg::InitStatus )
        {
            return KFMsg::AchieveNotDone;
        }

        if ( status == KFMsg::ReceiveStatus )
        {
            return KFMsg::AchieveAlreadyReceived;
        }

        // 设置已经领取
        player->UpdateObject( achieve_data, __STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );

        // 添加奖励
        player->AddElement( &setting->_reward, _default_multiple, __STRING__( achieve ), setting->_id, __FUNC_LINE__ );
        return KFMsg::AchieveReceiveOk;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __UPDATE_ACHIEVE_LIST__( update_function )\
    std::set<DataPtr> _update_list;\
    auto achieve_record = player->Find( __STRING__( achieve ) );\
    for ( auto achieve_data = achieve_record->First(); achieve_data != nullptr; achieve_data = achieve_record->Next() )\
    {\
        auto achieve_id = achieve_data->Get<uint32>( __STRING__( id ) );\
        auto setting = KFAchieveConfig::Instance()->FindSetting( achieve_id );\
        if ( setting == nullptr )\
        {\
            continue;\
        }\
        auto status = achieve_data->Get<uint32>( __STRING__( status ) );\
        if ( status != KFMsg::InitStatus )\
        {\
            continue;\
        }\
        _update_list.insert( achieve_data );\
    }\
    for ( auto achieve_data : _update_list )\
    {\
        auto condition_object = achieve_data->Find( __STRING__( conditions ) );\
        auto complete = update_function;\
        if ( complete )\
        {\
            player->UpdateObject( achieve_data, __STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFAchieveModule::OnAddDataAchieveModule )
    {
        __UPDATE_ACHIEVE_LIST__( _kf_condition->UpdateAddCondition( player, condition_object, data ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFAchieveModule::OnRemoveDataAchieveModule )
    {
        __UPDATE_ACHIEVE_LIST__( _kf_condition->UpdateRemoveCondition( player, condition_object, data ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFAchieveModule::OnUpdateDataAchieveModule )
    {
        __UPDATE_ACHIEVE_LIST__( _kf_condition->UpdateUpdateCondition( player, condition_object, data, operate, old_value, new_value ) );
    }
}