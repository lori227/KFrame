#include "KFResetModule.hpp"

namespace KFrame
{
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

    __KF_RUN_PLAYER_FUNCTION__( KFResetModule::RunResetPlayerData )
    {
        if ( !_need_to_reset )
        {
            return;
        }

        ResetPlayerData( player );
    }

    __KF_RESET_PLAYER_FUNCTION__( KFResetModule::ResetPlayerData )
    {
        auto kfobject = player->GetData();
        auto kflastdata = kfobject->FindData( __KF_STRING__( resettime ) );
        auto lasttime = kflastdata->GetValue();

        KFDate lastdate( lasttime );
        KFDate nowdate( KFGlobal::Instance()->_real_time );

        // 需要重置的属性
        ResetConfigData( player, lastdate, nowdate );

        // 需要重置的逻辑
        ResetPlayerLogic( player, lastdate, nowdate );

        // 纪录时间
        player->UpdateData( kflastdata, KFEnum::Set, KFGlobal::Instance()->_real_time );
    }

    void KFResetModule::ResetConfigData( KFEntity* player, KFDate& lastdate, KFDate& nowdate )
    {
        for ( auto& iter : KFResetConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_time_setting == nullptr )
            {
                continue;
            }

            if ( !KFDate::CheckTime( &kfsetting->_time_setting->_time_data, lastdate, nowdate ) )
            {
                continue;
            }

            // 重置属性
            if ( kfsetting->_parent_name.empty() )
            {
                player->UpdateData( kfsetting->_data_name, kfsetting->_operate, kfsetting->_value );
            }
            else if ( kfsetting->_data_name.empty() )
            {
                player->RemoveData( kfsetting->_parent_name );
            }
            else if ( kfsetting->_key == 0u )
            {
                player->UpdateData( kfsetting->_parent_name, kfsetting->_data_name, kfsetting->_operate, kfsetting->_value );
            }
            else
            {
                player->UpdateData( kfsetting->_parent_name, kfsetting->_key, kfsetting->_data_name, kfsetting->_operate, kfsetting->_value );
            }
        }
    }

    void KFResetModule::ResetPlayerLogic( KFEntity* player, KFDate& lastdate, KFDate& nowdate )
    {
        for ( auto& iter : _reset_data_list._objects )
        {
            auto kfresetdata = iter.second;
            if ( !KFDate::CheckTime( &kfresetdata->_time_data, lastdate, nowdate ) )
            {
                continue;
            }

            // 直接回调函数
            if ( kfresetdata->_count <= 1u || player->IsInited() )
            {
                kfresetdata->_function( player, kfresetdata->_time_data, lastdate.GetTime(), nowdate.GetTime() );
            }
            else
            {
                ResetPlayerLogicCount( player, kfresetdata, lastdate, nowdate );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFResetModule::AddResetFunction( const KFTimeData& timedata, uint32 count, const std::string& module, KFResetFunction& function )
    {
        auto kfresetdata = _reset_data_list.Create( module );
        kfresetdata->_count = count;
        kfresetdata->_time_data = timedata;
        kfresetdata->_function = function;
    }

    void KFResetModule::RemoveResetFunction( const std::string& module )
    {
        _reset_data_list.Remove( module );
    }

    void KFResetModule::ResetPlayerLogicCount( KFEntity* player, KFResetData* kfresetdata, KFDate& lastdate, KFDate& nowdate )
    {
        // 计算出n周期前的时间
        auto nowtime = KFDate::CalcTimeData( &kfresetdata->_time_data, nowdate.GetTime() );
        auto calctime = KFDate::CalcTimeData( &kfresetdata->_time_data, nowtime, 0 - ( int32 )kfresetdata->_count );

        // 上次重置时间
        auto lasttime = KFDate::CalcTimeData( &kfresetdata->_time_data, lastdate.GetTime() );
        lasttime = __MAX__( lasttime, calctime );

        for ( auto i = 0u; i < kfresetdata->_count; ++i )
        {
            auto resettime = KFDate::CalcTimeData( &kfresetdata->_time_data, lasttime, 1 );
            if ( resettime > nowtime )
            {
                break;
            }

            // 重置函数
            kfresetdata->_function( player, kfresetdata->_time_data, lasttime, resettime );
            lasttime = resettime;
        }
    }
}