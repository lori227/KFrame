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
        if ( lasttime == 0u )
        {
            // 新号不执行重置逻辑
            kflastdata->SetValue( KFGlobal::Instance()->_real_time );
            return;
        }

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
            if ( !KFDate::CheckTime( &kfsetting->_time_data, lastdate, nowdate ) )
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

            switch ( kfresetdata->_type )
            {
            case KFResetEnum::Once:
                ResetOncePlayerLogic( player, kfresetdata, lastdate, nowdate );
                break;
            case KFResetEnum::Daily:
                ResetDailyPlayerLogic( player, kfresetdata, lastdate, nowdate );
                break;
            default:
                break;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFResetModule::AddResetFunction( const KFTimeData& timedata, uint32 type, uint32 count, const std::string& module, KFResetFunction& function )
    {
        auto kfresetdata = _reset_data_list.Create( module );
        kfresetdata->_time_data = timedata;
        kfresetdata->_type = type;
        kfresetdata->_count = count;
        kfresetdata->_function = function;
    }

    void KFResetModule::RemoveResetFunction( const std::string& module )
    {
        _reset_data_list.Remove( module );
    }

    void KFResetModule::ResetOncePlayerLogic( KFEntity* player, const KFResetData* kfresetdata, KFDate& lastdate, KFDate& nowdate )
    {
        // 直接回调函数
        kfresetdata->_function( player, lastdate.GetTime(), nowdate.GetTime() );
    }

    void KFResetModule::ResetDailyPlayerLogic( KFEntity* player, const KFResetData* kfresetdata, KFDate& lastdate, KFDate& nowdate )
    {
        // 计算上一次的时间
        uint64 nowtime = nowdate.GetTime();
        for ( auto i = 0u; i < kfresetdata->_count; ++i )
        {
            auto lasttime = KFDate::GetLastTime( &kfresetdata->_time_data, nowtime );
            kfresetdata->_function( player, lasttime, nowtime );
            nowtime = lasttime;

            if ( lasttime == 0u || !KFDate::CheckTime( &kfresetdata->_time_data, lastdate.GetTime(), lasttime ) )
            {
                break;
            }
        }
    }
}