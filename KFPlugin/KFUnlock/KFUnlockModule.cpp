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
        for ( auto& iter : KFUnlockConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto kfdatarecord = player->Find( kfsetting->_data_name );
            if ( kfdatarecord == nullptr || !CheckNeedUnlock( player, kfsetting, kfdatarecord ) )
            {
                continue;
            }

            if ( kfsetting->_unlock_condition->_ids.empty() )
            {
                UnlockPlayerData( player, kfsetting, kfdatarecord );
                continue;
            }

            // 已经在解锁列表中
            auto kfunlock = kfunlockrecord->Find( kfsetting->_id );
            if ( kfunlock != nullptr )
            {
                continue;
            }

            kfunlock = player->CreateData( kfunlockrecord );
            auto kfconditionobject = kfunlock->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, kfconditionobject, kfsetting->_unlock_condition );
            kfunlockrecord->Add( kfsetting->_id, kfunlock );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, kfconditionobject, false );
            if ( complete )
            {
                kfunlockrecord->Remove( kfsetting->_id );
                UnlockPlayerData( player, kfsetting, kfdatarecord );
            }
        }
    }

    void KFUnlockModule::UnlockPlayerData( KFEntity* player, const KFUnlockSetting* kfsetting )
    {
        auto kfdatarecord = player->Find( kfsetting->_data_name );
        if ( kfdatarecord == nullptr || !CheckNeedUnlock( player, kfsetting, kfdatarecord ) )
        {
            return;
        }

        UnlockPlayerData( player, kfsetting, kfdatarecord );
    }

    bool KFUnlockModule::CheckNeedUnlock( KFEntity* player, const KFUnlockSetting* kfsetting, DataPtr kfdatarecord )
    {
        auto kfdata = kfdatarecord->Find( kfsetting->_data_key );
        if ( kfsetting->_child_name.empty() )
        {
            if ( kfdata != nullptr )
            {
                return false;
            }
        }
        else
        {
            if ( kfdata != nullptr && kfdata->Get<uint32>( kfsetting->_child_name ) != 0u )
            {
                return false;
            }
        }

        return true;
    }

    void KFUnlockModule::UnlockPlayerData( KFEntity* player, const KFUnlockSetting* kfsetting, DataPtr kfdatarecord )
    {
        auto kfdata = kfdatarecord->Find( kfsetting->_data_key );
        if ( kfsetting->_child_name.empty() )
        {
            if ( kfdata == nullptr )
            {
                kfdata = player->CreateData( kfdatarecord );
                kfdata->Set( __STRING__( unlock ), KFGlobal::Instance()->_real_time );
                kfdata->Set( kfdatarecord->_data_setting->_value_key_name, kfsetting->_data_value );
                player->AddRecord( kfdatarecord, kfsetting->_data_key, kfdata );
            }
        }
        else
        {
            if ( kfdata != nullptr )
            {
                auto value = kfdata->Get<uint32>( kfsetting->_child_name );
                if ( value == 0u )
                {
                    player->UpdateObject( kfdata, kfsetting->_child_name, KFEnum::Set, kfsetting->_data_value );
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
        auto kfsetting = KFUnlockConfig::Instance()->FindSetting( unlockid );\
        if ( kfsetting == nullptr )\
        {\
            removes.push_back( unlockid );\
        }\
        else\
        {\
            _update_lock.insert( kfsetting ); \
        }\
    }\
    for ( auto unlockid : removes )\
    {\
        kfunlockrecord->Remove( unlockid );\
    }\
    for ( auto kfsetting : _update_lock )\
    {\
        auto kfunlock = kfunlockrecord->Find( kfsetting->_id );\
        if ( kfunlock == nullptr )\
        {\
            continue;\
        }\
        auto kfconditionobject = kfunlock->Find( __STRING__( conditions ) );\
        auto complete = updatefunction;\
        if ( complete )\
        {\
            kfunlockrecord->Remove( kfsetting->_id );\
            UnlockPlayerData( player, kfsetting );\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFUnlockModule::OnAddDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateAddCondition( player, kfconditionobject, kfdata ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFUnlockModule::OnRemoveDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateRemoveCondition( player, kfconditionobject, kfdata ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFUnlockModule::OnUpdateDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateUpdateCondition( player, kfconditionobject, kfdata, operate, value, newvalue ) );
    }
}

