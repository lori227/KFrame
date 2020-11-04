#include "KFResetModule.hpp"

namespace KFrame
{
    void KFResetModule::BeforeRun()
    {
        __REGISTER_PLAYER_RESET__( &KFResetModule::ResetPlayerData );
        __REGISTER_PLAYER_RUN__( &KFResetModule::RunResetPlayerData );
        __REGISTER_PLAYER_ENTER__( &KFResetModule::OnEnterResetModule );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFResetModule::ShutDown()
    {
        __UN_PLAYER_RUN__();
        __UN_PLAYER_RESET__();
        __UN_PLAYER_ENTER__();
    }

    void KFResetModule::AddResetFunction( const std::string& functionname, uint32 count, KFResetFunction& function )
    {
        auto kfresetdata = _reset_logic_list.Create( functionname );
        kfresetdata->_count = count;
        kfresetdata->_function = function;
    }

    void KFResetModule::RemoveResetFunction( const std::string& functionname )
    {
        _reset_logic_list.Remove( functionname );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFResetModule::OnEnterResetModule )
    {
        // 把列表中不存在的时间数据删除掉
        UInt32List removes;
        auto kftimerecord = player->Find( __STRING__( time ) );
        for ( auto kftime = kftimerecord->First(); kftime != nullptr; kftime = kftimerecord->Next() )
        {
            auto id = kftime->GetKeyID();
            auto kfsetting = KFTimeConfig::Instance()->FindSetting( id );
            if ( kfsetting == nullptr )
            {
                removes.push_back( id );
            }
        }

        for ( auto id : removes )
        {
            kftimerecord->Remove( id );
        }
    }

    UInt64Map& KFResetModule::UpdateAllResetTime( KFEntity* player, KFData* kftimerecord )
    {
        static UInt64Map _time_id_list;
        _time_id_list.clear();

        for ( auto& iter : KFTimeConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_type == KFTimeEnum::Loop )
            {
                auto ok = false;
                auto lasttime = _invalid_int;
                std::tie( ok, lasttime ) = UpdateResetTime( player, kftimerecord, kfsetting );
                if ( ok )
                {
                    _time_id_list[ iter.first ] = lasttime;
                }
            }
        }

        return _time_id_list;
    }

    std::tuple<bool, uint64> KFResetModule::UpdateResetTime( KFEntity* player, KFData* kftimerecord, const KFTimeSetting* kfsetting )
    {
        auto kftime = kftimerecord->Find( kfsetting->_id );
        if ( kftime == nullptr )
        {
            kftime = player->CreateData( kftimerecord );
            kftimerecord->Add( kfsetting->_id, kftime );
        }

        auto nexttime = kftime->Get<uint64>( __STRING__( value ) );
        auto ok = KFDate::CheckPassTime( KFGlobal::Instance()->_real_time, nexttime );
        if ( ok )
        {
            auto timedata = &kfsetting->_time_section_list.front()._start_time;
            auto newtime = KFDate::CalcTimeData( timedata, KFGlobal::Instance()->_real_time, 1 );
            kftime->Set( __STRING__( value ), newtime );
        }

        return std::make_tuple( ok, nexttime );
    }

    uint64 KFResetModule::CalcNextResetTime( KFEntity* player, uint32 timeid )
    {
        return player->Get( __STRING__( time ), timeid, __STRING__( value ) );
    }

    void KFResetModule::SetResetTime( KFEntity* player, uint32 timeid, uint64 nowtime )
    {
        if ( timeid == 0u )
        {
            return;
        }

        auto kftimerecord = player->Find( __STRING__( time ) );
        player->UpdateRecord( kftimerecord, timeid, kftimerecord->_data_setting->_value_key_name, KFEnum::Set, nowtime );
    }

    __KF_PLAYER_RUN_FUNCTION__( KFResetModule::RunResetPlayerData )
    {
        auto kfresetdata = player->Find( __STRING__( resettime ) );
        auto resettime = kfresetdata->Get();
        if ( resettime > KFGlobal::Instance()->_game_time )
        {
            return;
        }
        kfresetdata->Set( KFGlobal::Instance()->_game_time + KFTimeEnum::OneMinuteMicSecond );

        // 玩家重置逻辑
        ResetPlayerData( player );
    }

    __KF_PLAYER_RESET_FUNCTION__( KFResetModule::ResetPlayerData )
    {
        // 判断所有时间id, 是否有需要更新
        auto kftimerecord = player->Find( __STRING__( time ) );
        auto& timeidlist = UpdateAllResetTime( player, kftimerecord );
        for ( auto& iter  : timeidlist )
        {
            auto kfsetting = KFResetConfig::Instance()->FindSetting( iter.first );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            for ( auto& resetdata : kfsetting->_reset_data_list )
            {
                if ( resetdata._function_name.empty() )
                {
                    ResetPlayerData( player, &resetdata );
                }
                else
                {
                    ResetPlayerLogic( player, iter.first, iter.second, resetdata._function_name );
                }
            }
        }
    }

    void KFResetModule::ResetPlayerData( KFEntity* player, const KFResetData* resetdata )
    {
        if ( resetdata->_parent_name.empty() )
        {
            player->UpdateData( resetdata->_data_name, resetdata->_operate, resetdata->_value );
        }
        else if ( resetdata->_data_name.empty() )
        {
            if ( resetdata->_key == 0u )
            {
                player->ClearRecord( resetdata->_parent_name );
            }
            else
            {
                player->RemoveRecord( resetdata->_parent_name, resetdata->_key );
            }
        }
        else if ( resetdata->_key == 0u )
        {
            player->UpdateObject( resetdata->_parent_name, resetdata->_data_name, resetdata->_operate, resetdata->_value );
        }
        else
        {
            player->UpdateRecord( resetdata->_parent_name, resetdata->_key, resetdata->_data_name, resetdata->_operate, resetdata->_value );
        }
    }

    void KFResetModule::ResetPlayerLogic( KFEntity* player, uint32 timeid, uint64 lastresettime, const std::string& functionname )
    {
        auto resetlogicdata = _reset_logic_list.Find( functionname );
        if ( resetlogicdata == nullptr )
        {
            return;
        }

        auto kftimesetting = KFTimeConfig::Instance()->FindSetting( timeid );
        if ( kftimesetting == nullptr )
        {
            return;
        }
        auto timedata = &kftimesetting->_time_section_list.front()._start_time;
        auto nowresettime = KFDate::CalcTimeData( timedata, KFGlobal::Instance()->_real_time );

        // 如果只计算一次
        if ( resetlogicdata->_count <= 1u || player->IsInited() || lastresettime == 0u )
        {
            return resetlogicdata->_function( player, timeid, lastresettime, nowresettime );
        }

        // 计算多次
        // 计算出n周期前的时间
        auto calcresettime = KFDate::CalcTimeData( timedata, nowresettime, 0 - ( int32 )resetlogicdata->_count );

        // 上次重置时间
        lastresettime = KFDate::CalcTimeData( timedata, lastresettime, -1 );
        lastresettime = __MAX__( lastresettime, calcresettime );

        for ( auto i = 0u; i < resetlogicdata->_count; ++i )
        {
            auto resettime = KFDate::CalcTimeData( timedata, lastresettime, 1 );
            if ( resettime > nowresettime )
            {
                break;
            }

            // 重置函数
            resetlogicdata->_function( player, timeid, lastresettime, resettime );
            lastresettime = resettime;
        }

    }
}