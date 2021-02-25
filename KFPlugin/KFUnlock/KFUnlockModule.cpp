#include "KFUnlockModule.hpp"

namespace KFrame
{
    void KFUnlockModule::BeforeRun()
    {
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_DATA__( &KFUnlockModule::OnAddDataUnlockModule );
        __REGISTER_REMOVE_DATA__( &KFUnlockModule::OnRemoveDataUnlockModule );
        __REGISTER_UPDATE_DATA__( &KFUnlockModule::OnUpdateDataUnlockModule );

        __REGISTER_PLAYER_ENTER__( &KFUnlockModule::OnEnterUnlockModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFUnlockModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_REMOVE_DATA__();
        __UN_UPDATE_DATA__();

        __UN_PLAYER_ENTER__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFUnlockModule::OnEnterUnlockModule )
    {
        auto unlock_record = player->Find( __STRING__( unlock ) );
        for ( auto& iter : KFUnlockConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            auto record_data = player->Find( setting->_data_name );
            if ( record_data == nullptr || !CheckNeedUnlock( player, setting, record_data ) )
            {
                continue;
            }

            if ( setting->_unlock_condition->_id_list.empty() )
            {
                UnlockPlayerData( player, setting, record_data );
                continue;
            }

            // 已经在解锁列表中
            auto unlock_data = unlock_record->Find( setting->_id );
            if ( unlock_data != nullptr )
            {
                continue;
            }

            unlock_data = player->CreateData( unlock_record );
            auto condition_object = unlock_data->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, condition_object, setting->_unlock_condition );
            unlock_record->Add( setting->_id, unlock_data );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, condition_object, false );
            if ( complete )
            {
                unlock_record->Remove( setting->_id );
                UnlockPlayerData( player, setting, record_data );
            }
        }
    }

    void KFUnlockModule::UnlockPlayerData( EntityPtr player, std::shared_ptr<const KFUnlockSetting> setting )
    {
        auto record_data = player->Find( setting->_data_name );
        if ( record_data == nullptr || !CheckNeedUnlock( player, setting, record_data ) )
        {
            return;
        }

        UnlockPlayerData( player, setting, record_data );
    }

    bool KFUnlockModule::CheckNeedUnlock( EntityPtr player, std::shared_ptr<const KFUnlockSetting> setting, DataPtr record_data )
    {
        auto data = record_data->Find( setting->_data_key );
        if ( setting->_child_name.empty() )
        {
            if ( data != nullptr )
            {
                return false;
            }
        }
        else
        {
            if ( data != nullptr && data->Get<uint32>( setting->_child_name ) != 0u )
            {
                return false;
            }
        }

        return true;
    }

    void KFUnlockModule::UnlockPlayerData( EntityPtr player, std::shared_ptr<const KFUnlockSetting> setting, DataPtr record_data )
    {
        auto data = record_data->Find( setting->_data_key );
        if ( setting->_child_name.empty() )
        {
            if ( data == nullptr )
            {
                data = player->CreateData( record_data );
                data->Operate( __STRING__( unlock ), KFEnum::Set, KFGlobal::Instance()->_real_time );
                data->Operate( record_data->_data_setting->_value_key_name, KFEnum::Set, setting->_data_value );
                player->AddRecord( record_data, setting->_data_key, data );
            }
        }
        else
        {
            if ( data != nullptr )
            {
                auto value = data->Get<uint32>( setting->_child_name );
                if ( value == 0u )
                {
                    player->UpdateObject( data, setting->_child_name, KFEnum::Set, setting->_data_value );
                }
            }
        }
    }

#define __UPDATE_UNLOCK_LIST__( update_function )\
    UInt32List remove_list;\
    std::set<std::shared_ptr<const KFUnlockSetting>> _unlock_setting_list;\
    auto unlock_record = player->Find( __STRING__( unlock ) );\
    for ( auto unlock_data = unlock_record->First(); unlock_data != nullptr; unlock_data = unlock_record->Next() )\
    {\
        auto unlock_id = unlock_data->Get<uint32>( __STRING__( id ) );\
        auto setting = KFUnlockConfig::Instance()->FindSetting( unlock_id );\
        if ( setting == nullptr )\
        {\
            remove_list.push_back( unlock_id );\
        }\
        else\
        {\
            _unlock_setting_list.insert( setting ); \
        }\
    }\
    for ( auto unlock_id : remove_list )\
    {\
        unlock_record->Remove( unlock_id );\
    }\
    for ( auto setting : _unlock_setting_list )\
    {\
        auto unlock_data = unlock_record->Find( setting->_id );\
        if ( unlock_data == nullptr )\
        {\
            continue;\
        }\
        auto condition_object = unlock_data->Find( __STRING__( conditions ) );\
        auto complete = update_function;\
        if ( complete )\
        {\
            unlock_record->Remove( setting->_id );\
            UnlockPlayerData( player, setting );\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFUnlockModule::OnAddDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateAddCondition( player, condition_object, data ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFUnlockModule::OnRemoveDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateRemoveCondition( player, condition_object, data ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFUnlockModule::OnUpdateDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateUpdateCondition( player, condition_object, data, operate, value, new_value ) );
    }
}

