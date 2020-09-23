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

    bool KFResetModule::UpdateAllResetTime( KFEntity* player, KFData* kftimerecord )
    {
        bool reset = false;
        for ( auto& iter : KFTimeConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_type == KFTimeEnum::Loop )
            {
                reset |= UpdateResetTime( player, kftimerecord, kfsetting );
            }
        }

        return reset;
    }

    bool KFResetModule::UpdateResetTime( KFEntity* player, KFData* kftimerecord, const KFTimeSetting* kfsetting )
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
            kftime->Set( __STRING__( status ), 1u );

            auto timedata = &kfsetting->_time_section_list.front()._start_time;
            nexttime = KFDate::CalcTimeData( timedata, KFGlobal::Instance()->_real_time, 1 );
            kftime->Set( __STRING__( value ), nexttime );
        }
        else
        {
            kftime->Set( __STRING__( status ), 0u );
        }

        return ok;
    }

    bool KFResetModule::CheckResetTime( KFEntity* player, uint32 timeid )
    {
        auto kftimerecord = player->Find( __STRING__( time ) );
        return CheckResetTimeData( kftimerecord, timeid );
    }

    bool KFResetModule::CheckResetTimeData( KFData* kftimerecord, uint32 timeid )
    {
        auto status = kftimerecord->Get<uint32>( timeid, __STRING__( status ) );
        return status == 1u;
    }

    uint64 KFResetModule::CalcNextResetTime( uint64 time, uint32 timeid )
    {
        auto kfsetting = KFTimeConfig::Instance()->FindSetting( timeid );
        if ( kfsetting == nullptr )
        {
            return 0u;
        }

        auto timedata = &kfsetting->_time_section_list.front()._start_time;
        return KFDate::CalcTimeData( timedata, time, 1 );
    }

    void KFResetModule::ResetTime( KFEntity* player, uint32 timeid )
    {
        if ( timeid == 0u )
        {
            return;
        }

        auto kftimerecord = player->Find( __STRING__( time ) );
        player->UpdateRecordData( kftimerecord, timeid, kftimerecord->_data_setting->_value_key_name, KFEnum::Set, KFGlobal::Instance()->_real_time );
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
        auto ok = UpdateAllResetTime( player, kftimerecord );
        if ( !ok )
        {
            return;
        }

        // 需要重置的属性
        ResetConfigData( player, kftimerecord );

        // 需要重置的逻辑
        ResetPlayerLogic( player, kftimerecord );
    }

    void KFResetModule::ResetConfigData( KFEntity* player, KFData* kftimerecord )
    {
        for ( auto& iter : KFResetConfig::Instance()->_reset_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !CheckResetTimeData( kftimerecord, iter.first ) )
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
                    player->CleanRecordData( resetdata._parent_name );
                }
                else if ( resetdata._key == 0u )
                {
                    player->UpdateObjectData( resetdata._parent_name, resetdata._data_name, resetdata._operate, resetdata._value );
                }
                else
                {
                    player->UpdateRecordData( resetdata._parent_name, resetdata._key, resetdata._data_name, resetdata._operate, resetdata._value );
                }
            }
        }
    }

    void KFResetModule::ResetPlayerLogic( KFEntity* player, KFData* kftimerecord )
    {
        static KFTimeData _time_data;
        for ( auto& iter : _reset_data_list._objects )
        {
            auto kfresetsetting = iter.second;

            uint64 lasttime = 0u;
            const KFTimeData* timedata = &_time_data;
            if ( kfresetsetting->_time_setting != nullptr )
            {
                if ( !CheckResetTimeData( kftimerecord, kfresetsetting->_time_setting->_id ) )
                {
                    continue;
                }

                timedata = &kfresetsetting->_time_setting->_time_section_list.front()._start_time;
                lasttime = kftimerecord->Get<uint64>( kfresetsetting->_time_setting->_id, __STRING__( lasttime ) );
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
                    resetdata->_function( player, timedata, lasttime, KFGlobal::Instance()->_real_time );
                }
                else
                {
                    ResetPlayerLogicCount( player, timedata, resetdata, lasttime, KFGlobal::Instance()->_real_time );
                }
            }
        }
    }

    void KFResetModule::ResetPlayerLogicCount( KFEntity* player, const KFTimeData* timedata, const KFResetLogicData* resetdata, uint64 lasttime, uint64 nowtime )
    {
        if ( lasttime == 0u )
        {
            return;
        }

        // 计算出n周期前的时间
        auto nowresettime = KFDate::CalcTimeData( timedata, nowtime );
        auto calcresettime = KFDate::CalcTimeData( timedata, nowresettime, 0 - ( int32 )resetdata->_count );

        // 上次重置时间
        auto lastresettime = KFDate::CalcTimeData( timedata, lasttime );
        lastresettime = __MAX__( lastresettime, calcresettime );

        for ( auto i = 0u; i < resetdata->_count; ++i )
        {
            auto resettime = KFDate::CalcTimeData( timedata, lastresettime, 1 );
            if ( resettime > nowresettime )
            {
                break;
            }

            // 重置函数
            resetdata->_function( player, timedata, lastresettime, resettime );
            lastresettime = resettime;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFResetModule::AddResetFunction( uint32 timeid, uint32 count, const std::string& module, KFResetFunction& function )
    {
        auto kfresetsetting = _reset_data_list.Create( timeid );
        kfresetsetting->_time_setting = KFTimeConfig::Instance()->FindSetting( timeid );
        if ( kfresetsetting->_time_setting == nullptr )
        {
            if ( timeid != 0u )
            {
                __LOG_ERROR__( "module=[{}] timeid=[{}] can't find setting", module, timeid );
            }
        }

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