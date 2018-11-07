#include "KFResetModule.h"
#include "KFResetConfig.h"

namespace KFrame
{
    KFResetModule::KFResetModule()
    {
        _need_to_reset = false;
        _next_reset_data_time = 0;
    }

    KFResetModule::~KFResetModule()
    {
    }

    void KFResetModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_reset_config, true );
    }

    void KFResetModule::BeforeRun()
    {
        CalcNextResetTime();

        _kf_player->RegisterResetFunction( this, &KFResetModule::ResetPlayerData );
        _kf_player->RegisterRunDataFunction( this, &KFResetModule::RunResetPlayerData );
    }

    void KFResetModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();
        _kf_player->UnRegisterResetFunction( this );
        _kf_player->UnRegisterRunDataFunction( this );
    }

    void KFResetModule::AfterRun()
    {
        _need_to_reset = CheckResetPlayerDataTime();
    }

    void KFResetModule::CalcNextResetTime()
    {
        auto nowtime = KFGlobal::Instance()->_real_time;

        // 在整点的时候刷新一次
        _next_reset_data_time = ( ( nowtime + KFTimeEnum::OneHourSecond - 1 ) / KFTimeEnum::OneHourSecond ) * KFTimeEnum::OneHourSecond;
    }

    bool KFResetModule::CheckResetPlayerDataTime()
    {
        if ( KFGlobal::Instance()->_real_time < _next_reset_data_time )
        {
            return false;
        }

        _next_reset_data_time += KFTimeEnum::OneHourSecond;
        return true;
    }

    void KFResetModule::ResetPlayerData( KFEntity* player )
    {
        auto nowtime = KFGlobal::Instance()->_real_time;

        auto kfobject = player->GetData();
        auto kfnoterecord = kfobject->FindData( __KF_STRING__( note ) );

        for ( auto& iter : _kf_reset_config->_reset_setting._objects )
        {
            auto kfsetting = iter.second;

            auto lasttime = kfnoterecord->GetValue< uint64 >( kfsetting->_note_id, __KF_STRING__( value ) );
            if ( !KFDate::CheckTime( kfsetting->_time_type, kfsetting->_time_value, kfsetting->_time_hour, lasttime, nowtime ) )
            {
                continue;
            }

            // 保存时间
            player->UpdateData( kfnoterecord, kfsetting->_note_id, __KF_STRING__( value ), KFOperateEnum::Set, nowtime );

            // 重置变量
            for ( auto& resetdata : kfsetting->_reset_data_list )
            {
                auto kfreset = &resetdata;

                if ( kfreset->_parent_name.empty() )
                {
                    player->UpdateData( kfreset->_data_name, kfreset->_operate, kfreset->_value );
                }
                else if ( kfreset->_data_name.empty() )
                {
                    player->RemoveData( kfreset->_parent_name );
                }
                else if ( kfreset->_key == 0 )
                {
                    player->UpdateData( kfreset->_parent_name, kfreset->_data_name, kfreset->_operate, kfreset->_value );
                }
                else
                {
                    player->UpdateData( kfreset->_parent_name, kfreset->_key, kfreset->_data_name, kfreset->_operate, kfreset->_value );
                }
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFResetModule::RunResetPlayerData( KFEntity* player )
    {
        if ( !_need_to_reset )
        {
            return;
        }

        ResetPlayerData( player );
    }
}