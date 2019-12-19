#include "KFUnlockModule.hpp"

namespace KFrame
{
    void KFUnlockModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_DATA__( &KFUnlockModule::OnAddDataUnlockModule );
        __REGISTER_REMOVE_DATA__( &KFUnlockModule::OnRemoveDataUnlockModule );
        __REGISTER_UPDATE_DATA__( &KFUnlockModule::OnUpdateDataUnlockModule );

        __REGISTER_ENTER_PLAYER__( &KFUnlockModule::OnEnterUnlockModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFUnlockModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_REMOVE_DATA__();
        __UN_UPDATE_DATA__();

        __UN_ENTER_PLAYER__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFUnlockModule::OnEnterUnlockModule )
    {
        auto kfunlockrecord = player->Find( __STRING__( unlock ) );
        for ( auto& iter : KFUnlockConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto kfdatarecord = player->Find( kfsetting->_data_name );
            if ( kfdatarecord == nullptr )
            {
                continue;
            }

            auto kfdata = kfdatarecord->Find( kfsetting->_data_id );
            if ( kfdata != nullptr )
            {
                continue;
            }

            if ( kfsetting->_unlock_condition.empty() )
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
            _kf_condition->AddCondition( kfconditionobject, kfsetting->_unlock_condition, kfsetting->_condition_type );
            kfunlockrecord->Add( kfsetting->_id, kfunlock );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, kfconditionobject, KFConditionEnum::LimitNull, false );
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
        if ( kfdatarecord == nullptr )
        {
            return;
        }

        auto kfdata = kfdatarecord->Find( kfsetting->_data_id );
        if ( kfdata != nullptr )
        {
            return;
        }

        UnlockPlayerData( player, kfsetting, kfdatarecord );
    }

    void KFUnlockModule::UnlockPlayerData( KFEntity* player, const KFUnlockSetting* kfsetting, KFData* kfdatarecord )
    {
        auto kfdata = player->CreateData( kfdatarecord );
        kfdata->Set( __STRING__( unlock ), KFGlobal::Instance()->_real_time );
        kfdata->Set( kfdatarecord->_data_setting->_value_key_name, kfsetting->_data_value );
        player->AddData( kfdatarecord, kfsetting->_data_id, kfdata );
    }

#define __UPDATE_UNLOCK_LIST__( updatefunction )\
    UInt32List removes;\
    std::map< KFData*, const KFUnlockSetting* > _update_lock;\
    auto kfunlockrecord = player->Find( __STRING__( unlock ) );\
    for ( auto kfunlock = kfunlockrecord->First(); kfunlock != nullptr; kfunlock = kfunlockrecord->Next() )\
    {\
        auto unlockid = kfunlock->Get<uint32>( __STRING__( id ) );\
        auto kfsetting = KFUnlockConfig::Instance()->FindSetting( unlockid );\
        if ( kfsetting == nullptr )\
        {\
            removes.push_back( unlockid );\
            continue;\
        }\
        auto kfconditionobject = kfunlock->Find( __STRING__( conditions ) );\
        _update_lock[ kfconditionobject ] = kfsetting;\
    }\
    for ( auto unlockid : removes )\
    {\
        kfunlockrecord->Remove( unlockid );\
    }\
    for ( auto& iter : _update_lock )\
    {\
        auto kfconditionobject = iter.first;\
        auto kfsetting = iter.second;\
        auto complete = updatefunction;\
        if ( complete )\
        {\
            kfunlockrecord->Remove( kfsetting->_id );\
            UnlockPlayerData( player, kfsetting );\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFUnlockModule::OnAddDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateAddCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFUnlockModule::OnRemoveDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateRemoveCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFUnlockModule::OnUpdateDataUnlockModule )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateUpdateCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata, operate, value, newvalue ) );
    }
}

