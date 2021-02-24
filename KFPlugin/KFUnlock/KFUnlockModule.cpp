#include "KFUnlockModule.hpp"

namespace KFrame
{
    void KFUnlockModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
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
        auto kfunlockrecord = player->Find( __STRING__( unlock ) );
        for ( auto& iter : KFUnlockConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            auto kfdatarecord = player->Find( setting->_data_name );
            if ( kfdatarecord == nullptr || !CheckNeedUnlock( player, setting, kfdatarecord ) )
            {
                continue;
            }

            if ( setting->_unlock_condition->_id_list.empty() )
            {
                UnlockPlayerData( player, setting, kfdatarecord );
                continue;
            }

            // 已经在解锁列表中
            auto kfunlock = kfunlockrecord->Find( setting->_id );
            if ( kfunlock != nullptr )
            {
                continue;
            }

            kfunlock = player->CreateData( kfunlockrecord );
            auto condition_object = kfunlock->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, condition_object, setting->_unlock_condition );
            kfunlockrecord->Add( setting->_id, kfunlock );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, condition_object, false );
            if ( complete )
            {
                kfunlockrecord->Remove( setting->_id );
                UnlockPlayerData( player, setting, kfdatarecord );
            }
        }
    }

    void KFUnlockModule::UnlockPlayerData( EntityPtr player, const KFUnlockSetting* setting )
    {
        auto kfdatarecord = player->Find( setting->_data_name );
        if ( kfdatarecord == nullptr || !CheckNeedUnlock( player, setting, kfdatarecord ) )
        {
            return;
        }

        UnlockPlayerData( player, setting, kfdatarecord );
    }

    bool KFUnlockModule::CheckNeedUnlock( EntityPtr player, const KFUnlockSetting* setting, DataPtr kfdatarecord )
    {
        auto kfdata = kfdatarecord->Find( setting->_data_key );
        if ( setting->_child_name.empty() )
        {
            if ( kfdata != nullptr )
            {
                return false;
            }
        }
        else
        {
            if ( kfdata != nullptr && kfdata->Get<uint32>( setting->_child_name ) != 0u )
            {
                return false;
            }
        }

        return true;
    }

    void KFUnlockModule::UnlockPlayerData( EntityPtr player, const KFUnlockSetting* setting, DataPtr kfdatarecord )
    {
        auto kfdata = kfdatarecord->Find( setting->_data_key );
        if ( setting->_child_name.empty() )
        {
            if ( kfdata == nullptr )
            {
                kfdata = player->CreateData( kfdatarecord );
                kfdata->Set( __STRING__( unlock ), KFGlobal::Instance()->_real_time );
                kfdata->Set( kfdatarecord->_data_setting->_value_key_name, setting->_data_value );
                player->AddRecord( kfdatarecord, setting->_data_key, kfdata );
            }
        }
        else
        {
            if ( kfdata != nullptr )
            {
                auto value = kfdata->Get<uint32>( setting->_child_name );
                if ( value == 0u )
                {
                    player->UpdateObject( kfdata, setting->_child_name, KFEnum::Set, setting->_data_value );
                }
            }
        }
    }

#define __UPDATE_UNLOCK_LIST__( updatefunction )\
    UInt32List removes;\
    std::set< const KFUnlockSetting* > _update_lock;\
    auto kfunlockrecord = player->Find( __STRING__( unlock ) );\
    for ( auto kfunlock = kfunlockrecord->First(); kfunlock != nullptr; kfunlock = kfunlockrecord->Next() )\
    {\
        auto unlockid = kfunlock->Get<uint32>( __STRING__( id ) );\
        auto setting = KFUnlockConfig::Instance()->FindSetting( unlockid );\
        if ( setting == nullptr )\
        {\
            removes.push_back( unlockid );\
        }\
        else\
        {\
            _update_lock.insert( setting ); \
        }\
    }\
    for ( auto unlockid : removes )\
    {\
        kfunlockrecord->Remove( unlockid );\
    }\
    for ( auto setting : _update_lock )\
    {\
        auto kfunlock = kfunlockrecord->Find( setting->_id );\
        if ( kfunlock == nullptr )\
        {\
            continue;\
        }\
        auto condition_object = kfunlock->Find( __STRING__( conditions ) );\
        auto complete = updatefunction;\
        if ( complete )\
        {\
            kfunlockrecord->Remove( setting->_id );\
            UnlockPlayerData( player, setting );\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFUnlockModule::OnAddDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateAddCondition( player, condition_object, kfdata ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFUnlockModule::OnRemoveDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateRemoveCondition( player, condition_object, kfdata ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFUnlockModule::OnUpdateDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateUpdateCondition( player, condition_object, kfdata, operate, value, newvalue ) );
    }
}

