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
#ifdef __KF_DEBUG__
        _reset_loop_time = KFTimeEnum::OneMinuteSecond;
#else
        _reset_loop_time = KFTimeEnum::OneHourSecond;
#endif // __KF_DEBUG__

        // 在整点的时候刷新一次
        auto nowtime = KFGlobal::Instance()->_real_time;
        _next_reset_data_time = ( ( nowtime + _reset_loop_time - 1 ) / _reset_loop_time ) * _reset_loop_time;
    }

    bool KFResetModule::CheckResetPlayerDataTime()
    {
        if ( KFGlobal::Instance()->_real_time < _next_reset_data_time )
        {
            return false;
        }

        _next_reset_data_time += _reset_loop_time;
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
        auto kflastdata = player->Find( __STRING__( resettime ) );
        auto lasttime = kflastdata->Get();

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
        for ( auto& iter : KFResetConfig::Instance()->_reset_settings._objects )
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
            for ( auto& resetdata : kfsetting->_reset_data_list )
            {
                if ( resetdata._parent_name.empty() )
                {
                    player->UpdateData( resetdata._data_name, resetdata._operate, resetdata._value );
                }
                else if ( resetdata._data_name.empty() )
                {
                    player->CleanData( resetdata._parent_name );
                }
                else if ( resetdata._key == 0u )
                {
                    player->UpdateData( resetdata._parent_name, resetdata._data_name, resetdata._operate, resetdata._value );
                }
                else
                {
                    player->UpdateData( resetdata._parent_name, resetdata._key, resetdata._data_name, resetdata._operate, resetdata._value );
                }
            }
        }
    }

    void KFResetModule::ResetPlayerLogic( KFEntity* player, KFDate& lastdate, KFDate& nowdate )
    {
        static KFTimeData _time_data;
        for ( auto& iter : _reset_data_list._objects )
        {
            auto kfresetsetting = iter.second;
            const KFTimeData* timedata = &_time_data;
            if ( kfresetsetting->_time_setting != nullptr )
            {
                if ( !KFDate::CheckTime( &kfresetsetting->_time_setting->_time_data, lastdate, nowdate ) )
                {
                    continue;
                }

                timedata = &kfresetsetting->_time_setting->_time_data;
            }

            // 直接回调函数
            for ( auto resetdata : kfresetsetting->_reset_logic_data._objects )
            {
                if ( resetdata == nullptr )
                {
                    continue;
                }

                if ( resetdata->_count <= 1u || player->IsInited() )
                {
                    resetdata->_function( player, timedata, lastdate.GetTime(), nowdate.GetTime() );
                }
                else
                {
                    ResetPlayerLogicCount( player, timedata, resetdata, lastdate, nowdate );
                }
            }
        }
    }

    void KFResetModule::ResetPlayerLogicCount( KFEntity* player, const KFTimeData* timedata, const KFResetLogicData* kfresetdata, KFDate& lastdate, KFDate& nowdate )
    {
        if ( lastdate.GetTime() == 0u )
        {
            return;
        }

        // 计算出n周期前的时间
        auto nowtime = KFDate::CalcTimeData( timedata, nowdate.GetTime() );
        auto calctime = KFDate::CalcTimeData( timedata, nowtime, 0 - ( int32 )kfresetdata->_count );

        // 上次重置时间
        auto lasttime = KFDate::CalcTimeData( timedata, lastdate.GetTime() );
        lasttime = __MAX__( lasttime, calctime );

        for ( auto i = 0u; i < kfresetdata->_count; ++i )
        {
            auto resettime = KFDate::CalcTimeData( timedata, lasttime, 1 );
            if ( resettime > nowtime )
            {
                break;
            }

            // 重置函数
            kfresetdata->_function( player, timedata, lasttime, resettime );
            lasttime = resettime;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFResetModule::AddResetFunction( uint32 timeid, uint32 count, const std::string& module, KFResetFunction& function )
    {
        auto kfresetsetting = _reset_data_list.Create( timeid );
        auto kfsetting = KFTimeConfig::Instance()->FindSetting( timeid );
        if ( kfsetting == nullptr )
        {
            if ( timeid != 0u )
            {
                __LOG_ERROR__( "module=[{}] timeid=[{}] can't find setting!", module, timeid );
            }
        }
        kfresetsetting->_time_setting = kfsetting;

        auto kfresetdata = __KF_NEW__( KFResetLogicData );
        kfresetdata->_count = count;
        kfresetdata->_module = module;
        kfresetdata->_function = function;
        kfresetsetting->_reset_logic_data.Insert( kfresetdata );
    }

    void KFResetModule::RemoveResetFunction( const std::string& module )
    {
        for ( auto& iter : _reset_data_list._objects )
        {
            auto kfresettting = iter.second;
            for ( auto& kfresetdata : kfresettting->_reset_logic_data._objects )
            {
                if ( kfresetdata == nullptr || kfresetdata->_module != module )
                {
                    continue;
                }

                __KF_DELETE__( KFResetLogicData, kfresetdata );
                kfresetdata = nullptr;
            }
        }
    }
}