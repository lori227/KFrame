#include "KFUnlockModule.hpp"

namespace KFrame
{
    void KFUnlockModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_ADD_DATA__( &KFUnlockModule::OnAddDataUnlockModule );
        __REGISTER_UPDATE_DATA__( &KFUnlockModule::OnUpdateDataUnlockModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFUnlockModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_UPDATE_DATA__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_DATA_FUNCTION__( KFUnlockModule::OnAddDataUnlockModule )
    {
        UnlockPlayerData( player );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFUnlockModule::OnUpdateDataUnlockModule )
    {
        UnlockPlayerData( player );
    }

    void KFUnlockModule::UnlockPlayerData( KFEntity* player )
    {
        for ( auto& iter : KFUnlockConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto kfrecord = player->Find( kfsetting->_data_name );
            auto kfdata = kfrecord->Find( kfsetting->_data_id );
            if ( kfdata != nullptr )
            {
                continue;
            }

            if ( !_kf_kernel->CheckCondition( player, &kfsetting->_conditions ) )
            {
                continue;
            }

            kfdata = _kf_kernel->CreateObject( kfrecord->_data_setting );
            kfdata->Set( __KF_STRING__( unlock ), KFGlobal::Instance()->_real_time );
            kfdata->Set( kfrecord->_data_setting->_value_key_name, kfsetting->_data_value );
            player->AddData( kfrecord, kfsetting->_data_id, kfdata );
        }
    }
}

