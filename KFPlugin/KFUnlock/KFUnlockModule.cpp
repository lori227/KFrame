#include "KFUnlockModule.hpp"

namespace KFrame
{
    void KFUnlockModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_ADD_DATA__( &KFUnlockModule::OnAddDataUnlockModule );
        __REGISTER_UPDATE_DATA__( &KFUnlockModule::OnUpdateDataUnlockModule );

        __REGISTER_ENTER_PLAYER__( &KFUnlockModule::OnEnterUnlockModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFUnlockModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_UPDATE_DATA__();

        __UN_ENTER_PLAYER__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFUnlockModule::OnEnterUnlockModule )
    {
        auto kfunlockrecord = player->Find( __KF_STRING__( unlock ) );
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

            kfunlock = _kf_kernel->CreateObject( kfunlockrecord->_data_setting );
            _kf_condition->AddCondition( kfunlock, kfsetting->_unlock_condition );
            kfunlockrecord->Add( kfsetting->_id, kfunlock );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, kfunlock, kfsetting->_condition_type, KFConditionEnum::LimitNull, false );
            if ( complete )
            {
                UnlockPlayerData( player, kfsetting, kfdatarecord );
                kfunlockrecord->Remove( kfsetting->_id );
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

        auto kfdata = kfdatarecord->Find( kfsetting->_id );
        if ( kfdata != nullptr )
        {
            return;
        }

        UnlockPlayerData( player, kfsetting, kfdatarecord );
    }

    void KFUnlockModule::UnlockPlayerData( KFEntity* player, const KFUnlockSetting* kfsetting, KFData* kfdatarecord )
    {
        auto kfdata = _kf_kernel->CreateObject( kfdatarecord->_data_setting );
        kfdata->Set( __KF_STRING__( unlock ), KFGlobal::Instance()->_real_time );
        kfdata->Set( kfdatarecord->_data_setting->_value_key_name, kfsetting->_data_value );
        player->AddData( kfdatarecord, kfsetting->_data_id, kfdata );
    }

#define __GET_UNLOCK_LIST__()\
    ListUInt32 removes;\
    std::map< KFData*, const KFUnlockSetting* > _update_lock;\
    auto kfunlockrecord = player->Find( __KF_STRING__( unlock ) );\
    for ( auto kfunlock = kfunlockrecord->First(); kfunlock != nullptr; kfunlock = kfunlockrecord->Next() )\
    {\
        auto unlockid = kfunlock->Get<uint32>( __KF_STRING__( id ) );\
        auto kfsetting = KFUnlockConfig::Instance()->FindSetting( unlockid );\
        if ( kfsetting == nullptr )\
        {\
            removes.push_back( unlockid );\
            continue;\
        }\
        _update_lock[ kfunlock ] = kfsetting;\
    }\

    __KF_ADD_DATA_FUNCTION__( KFUnlockModule::OnAddDataUnlockModule )
    {
        __GET_UNLOCK_LIST__();
        for ( auto& iter : _update_lock )
        {
            auto kfunlock = iter.first;
            auto kfsetting = iter.second;

            // 判断触发
            auto complete = _kf_condition->UpdateAddCondition( player, kfunlock, kfsetting->_condition_type, KFConditionEnum::LimitNull, kfdata );
            if ( complete )
            {
                removes.push_back( kfsetting->_id );
                UnlockPlayerData( player, kfsetting );
            }
        }

        for ( auto unlockid : removes )
        {
            kfunlockrecord->Remove( unlockid );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFUnlockModule::OnUpdateDataUnlockModule )
    {
        __GET_UNLOCK_LIST__();
        for ( auto& iter : _update_lock )
        {
            auto kfunlock = iter.first;
            auto kfsetting = iter.second;

            // 判断触发
            auto complete = _kf_condition->UpdateUpdateCondition( player, kfunlock, kfsetting->_condition_type, KFConditionEnum::LimitNull, kfdata, operate, value, newvalue );
            if ( complete )
            {
                removes.push_back( kfsetting->_id );
                UnlockPlayerData( player, kfsetting );
            }
        }

        for ( auto unlockid : removes )
        {
            kfunlockrecord->Remove( unlockid );
        }
    }
}

