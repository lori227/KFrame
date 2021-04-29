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

    void KFResetModule::AddResetFunction( const std::string& function_name, uint32 count, KFModule* module, KFResetFunction& function )
    {
        auto reset_logic_data = _reset_logic_list.Create( function_name );
        reset_logic_data->_count = count;
        reset_logic_data->_function.SetFunction( module, function );
    }

    void KFResetModule::RemoveResetFunction( const std::string& function_name )
    {
        _reset_logic_list.Remove( function_name );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFResetModule::OnEnterResetModule )
    {
        // 把列表中不存在的时间数据删除掉
        UInt32List removes;
        auto time_record = player->Find( __STRING__( time ) );
        for ( auto time_data = time_record->First(); time_data != nullptr; time_data = time_record->Next() )
        {
            auto id = time_data->GetKeyID();
            auto setting = KFTimeLoopConfig::Instance()->FindSetting( id );
            if ( setting == nullptr )
            {
                removes.push_back( id );
            }
        }

        for ( auto id : removes )
        {
            time_record->Remove( id );
        }
    }

    UInt64Map& KFResetModule::UpdateAllResetTime( EntityPtr player, DataPtr time_record )
    {
        static UInt64Map _time_id_list;
        _time_id_list.clear();

        for ( auto& iter : KFTimeLoopConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            auto ok = false;
            auto last_time = _invalid_int;
            std::tie( ok, last_time ) = UpdateResetTime( player, time_record, setting );
            if ( ok )
            {
                _time_id_list[ iter.first ] = last_time;
            }
        }

        return _time_id_list;
    }

    std::tuple<bool, uint64> KFResetModule::UpdateResetTime( EntityPtr player, DataPtr time_record, std::shared_ptr<const KFTimeLoopSetting> setting )
    {
        auto time_data = time_record->Find( setting->_id );
        if ( time_data == nullptr )
        {
            time_data = player->CreateData( time_record );
            player->AddRecord( time_record, setting->_id, time_data );
        }

        auto next_time = time_data->Get<uint64>( __STRING__( value ) );
        auto ok = KFDate::CheckPassTime( KFGlobal::Instance()->_real_time, next_time );
        if ( ok )
        {
            auto new_time = KFDate::CalcTimeData( &setting->_time_data, KFGlobal::Instance()->_real_time, 1 );
            time_data->Operate( __STRING__( value ), KFEnum::Set, new_time );
        }

        return std::make_tuple( ok, next_time );
    }

    uint64 KFResetModule::CalcNextResetTime( EntityPtr player, uint32 time_id )
    {
        return player->Get( __STRING__( time ), time_id, __STRING__( value ) );
    }

    void KFResetModule::SetResetTime( EntityPtr player, uint32 time_id, uint64 now_time )
    {
        if ( time_id == 0u )
        {
            return;
        }

        auto time_record = player->Find( __STRING__( time ) );
        player->UpdateRecord( time_record, time_id, time_record->_data_setting->_value_key_name, KFEnum::Set, now_time );
    }

    __KF_PLAYER_RUN_FUNCTION__( KFResetModule::RunResetPlayerData )
    {
        auto reset_time_data = player->Find( __STRING__( resettime ) );
        auto reset_time = reset_time_data->Get();
        if ( reset_time > KFGlobal::Instance()->_game_time )
        {
            return;
        }
        reset_time_data->Set( KFGlobal::Instance()->_game_time + KFTimeEnum::OneMinuteMicSecond );

        // 玩家重置逻辑
        ResetPlayerData( player );
    }

    __KF_PLAYER_RESET_FUNCTION__( KFResetModule::ResetPlayerData )
    {
        // 判断所有时间id, 是否有需要更新
        auto time_record = player->Find( __STRING__( time ) );
        auto& time_id_list = UpdateAllResetTime( player, time_record );
        for ( auto& iter  : time_id_list )
        {
            auto setting = KFResetConfig::Instance()->FindSetting( iter.first );
            if ( setting == nullptr )
            {
                continue;
            }

            for ( auto& reset_data : setting->_reset_data )
            {
                if ( reset_data._function_name.empty() )
                {
                    ResetPlayerData( player, &reset_data );
                }
                else
                {
                    ResetPlayerLogic( player, iter.first, iter.second, reset_data._function_name );
                }
            }
        }
    }

    void KFResetModule::ResetPlayerData( EntityPtr player, const ResetData* reset_data )
    {
        if ( reset_data->_parent_name.empty() )
        {
            player->UpdateData( reset_data->_data_name, reset_data->_operate, reset_data->_value );
        }
        else if ( reset_data->_data_name.empty() )
        {
            if ( reset_data->_key == 0u )
            {
                player->ClearRecord( reset_data->_parent_name );
            }
            else
            {
                player->RemoveRecord( reset_data->_parent_name, reset_data->_key );
            }
        }
        else if ( reset_data->_key == 0u )
        {
            player->UpdateObject( reset_data->_parent_name, reset_data->_data_name, reset_data->_operate, reset_data->_value );
        }
        else
        {
            player->UpdateRecord( reset_data->_parent_name, reset_data->_key, reset_data->_data_name, reset_data->_operate, reset_data->_value );
        }
    }

    void KFResetModule::ResetPlayerLogic( EntityPtr player, uint32 time_id, uint64 last_reset_time, const std::string& function_name )
    {
        auto reset_logic_data = _reset_logic_list.Find( function_name );
        if ( reset_logic_data == nullptr )
        {
            return;
        }

        auto time_setting = KFTimeLoopConfig::Instance()->FindSetting( time_id );
        if ( time_setting == nullptr )
        {
            return;
        }
        auto time_data = &time_setting->_time_data;
        auto now_reset_time = KFDate::CalcTimeData( time_data, KFGlobal::Instance()->_real_time );

        // 如果只计算一次
        if ( reset_logic_data->_count <= 1u || player->IsInited() || last_reset_time == 0u )
        {
            return reset_logic_data->_function.Call( player, time_id, last_reset_time, now_reset_time );
        }

        // 计算多次
        // 计算出n周期前的时间
        auto calc_reset_time = KFDate::CalcTimeData( time_data, now_reset_time, 0 - ( int32 )reset_logic_data->_count );

        // 上次重置时间
        last_reset_time = KFDate::CalcTimeData( time_data, last_reset_time, -1 );
        last_reset_time = __MAX__( last_reset_time, calc_reset_time );

        for ( auto i = 0u; i < reset_logic_data->_count; ++i )
        {
            auto reset_time = KFDate::CalcTimeData( time_data, last_reset_time, 1 );
            if ( reset_time > now_reset_time )
            {
                break;
            }

            // 重置函数
            reset_logic_data->_function.Call( player, time_id, last_reset_time, reset_time );
            last_reset_time = reset_time;
        }

    }
}