#include "KFResetModule.hpp"

namespace KFrame
{
    void KFResetModule::InitModule()
    {
        __KF_ADD_CONFIG__( KFResetConfig );
    }

    void KFResetModule::BeforeRun()
    {
        __REGISTER_RESET_PLAYER__( &KFResetModule::ResetPlayerData );
        __REGISTER_RUN_PLAYER__( &KFResetModule::RunResetPlayerData );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        CalcNextResetTime();
    }

    void KFResetModule::ShutDown()
    {
        __UN_RESET_PLAYER__();
        __UN_RUN_PLAYER__();
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

    __KF_RESET_PLAYER_FUNCTION__( KFResetModule::ResetPlayerData )
    {
        auto nowtime = KFGlobal::Instance()->_real_time;

        auto kfobject = player->GetData();
        auto kfnoterecord = kfobject->FindData( __KF_STRING__( note ) );

        for ( auto& iter : KFResetConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;

            auto lasttime = kfnoterecord->GetValue( kfsetting->_id, __KF_STRING__( value ) );
            if ( !KFDate::CheckTime( kfsetting->_time_type, kfsetting->_time_value, kfsetting->_time_hour, lasttime, nowtime ) )
            {
                continue;
            }

            // 保存时间
            player->UpdateData( kfnoterecord, kfsetting->_id, __KF_STRING__( value ), KFEnum::Set, nowtime );

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
                else if ( kfreset->_key == _invalid_int )
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
    __KF_RUN_PLAYER_FUNCTION__( KFResetModule::RunResetPlayerData )
    {
        if ( !_need_to_reset )
        {
            return;
        }

        ResetPlayerData( player );
    }
}