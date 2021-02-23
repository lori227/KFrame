#include "KFTimerModule.hpp"

namespace KFrame
{
    KFTimerModule::KFTimerModule()
    {
        _now_slot = 0u;
        _total_run_use_time = 0u;
        memset( _slot_timer_data, 0u, sizeof( _slot_timer_data ) );
    }

    KFTimerModule::~KFTimerModule()
    {
        _module_timer_data_list.Clear();
        for ( auto i = 0u; i < TimerEnum::MaxSlot; ++i )
        {
            _slot_timer_data[i] = nullptr;
        }
    }

    void KFTimerModule::BeforeRun()
    {
        _now_slot = 0;
    }

    void KFTimerModule::Run()
    {
        RunTimerRemove();
        RunTimerRegister();
        RunTimerUpdate();
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<KFTimerData> KFTimerModule::FindTimerData( uint64 object_id, uint64 data_id, KFModule* module )
    {
        auto module_timer_data = _module_timer_data_list.Find( module );
        if ( module_timer_data == nullptr )
        {
            return nullptr;
        }

        return module_timer_data->FindTimerData( object_id, data_id );
    }

    bool KFTimerModule::AddTimerData( KFModule* module, std::shared_ptr<KFTimerData> timer_data )
    {
        auto module_timer_data = _module_timer_data_list.Create( module );
        return module_timer_data->AddTimerData( timer_data );
    }

    void KFTimerModule::RemoveRegisterTimerData( uint64 object_id, uint64 data_id, KFModule* module )
    {
        for ( auto iter = _register_timer_data.begin(); iter != _register_timer_data.end(); )
        {
            auto timer_data = *iter;
            if ( ( timer_data->_function.GetModule() == module ) &&
                    ( object_id == 0u || timer_data->_object_id == object_id ) &&
                    ( data_id == 0u || data_id == timer_data->_data_id ) )
            {
                iter = _register_timer_data.erase( iter );
            }
            else
            {
                ++iter;
            }
        }
    }

    bool KFTimerModule::RemoveTimerData( uint64 object_id, uint64 data_id, KFModule* module )
    {
        auto module_timer_data = _module_timer_data_list.Find( module );
        if ( module_timer_data == nullptr )
        {
            return false;
        }

        // 删除所有
        if ( object_id == 0u )
        {
            for ( auto& object_iter : module_timer_data->_object_list._objects )
            {
                auto object_data = object_iter.second;
                for ( auto& timer_iter : object_data->_timer_list._objects )
                {
                    RemoveSlotTimer( timer_iter.second );
                }
            }
            _module_timer_data_list.Remove( module );
        }
        else if ( data_id == 0 )
        {
            auto object_data = module_timer_data->RemoveObjectData( object_id );
            if ( object_data != nullptr )
            {
                for ( auto& iter : object_data->_timer_list._objects )
                {
                    RemoveSlotTimer( iter.second );
                }
            }
        }
        else
        {
            auto timer_data = module_timer_data->RemoveTimerData( object_id, data_id );
            if ( timer_data != nullptr )
            {
                RemoveSlotTimer( timer_data );
            }
        }

        return true;
    }

    void KFTimerModule::AddSlotTimer( std::shared_ptr<KFTimerData> timer_data )
    {
        auto ticks = 0u;
        if ( timer_data->_delay != 0u )
        {
            ticks = timer_data->_delay / TimerEnum::SlotTime;
            timer_data->_delay = 0u;
        }
        else
        {
            ticks = timer_data->_interval / TimerEnum::SlotTime;
        }

        // 设置圈数
        timer_data->_rotation = ticks / TimerEnum::MaxSlot;

        // _slot和_now_slot相同时, 会多跑一圈 slot+1 保证下次可以执行
        auto slot = ticks % TimerEnum::MaxSlot;
        timer_data->_slot = ( slot + _now_slot + 1u ) % TimerEnum::MaxSlot;

        auto wheel_data = _slot_timer_data[ timer_data->_slot ];
        if ( wheel_data != nullptr )
        {
            timer_data->_next = wheel_data;
            wheel_data->_prev = timer_data;
        }

        _slot_timer_data[ timer_data->_slot ] = timer_data;
    }

    void KFTimerModule::RemoveSlotTimer( std::shared_ptr<KFTimerData> timer_data )
    {
        auto prev_data = timer_data->_prev;
        if ( prev_data != nullptr )
        {
            prev_data->_next = timer_data->_next;
        }

        auto next_data = timer_data->_next;
        if ( next_data != nullptr )
        {
            next_data->_prev = timer_data->_prev;
        }

        if ( timer_data == _slot_timer_data[ timer_data->_slot ] )
        {
            _slot_timer_data[ timer_data->_slot ] = next_data;
        }

        timer_data->_prev = nullptr;
        timer_data->_next = nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTimerModule::AddLoopTimer( uint64 object_id, uint64 data_id, uint32 interval_time, uint32 delay_time, KFModule* module, KFTimerFunction& function )
    {
        if ( interval_time == 0u )
        {
            interval_time = __MAX_UINT32__;
            __LOG_ERROR__( "module=[{}] id=[{}] interval_time error", module->_plugin_name, object_id );
        }

        auto timer_data = __MAKE_SHARED__( KFTimerData );
        timer_data->_object_id = object_id;
        timer_data->_data_id = data_id;
        timer_data->_type = TimerEnum::Loop;
        timer_data->_delay = delay_time;
        timer_data->_interval = interval_time;
        timer_data->_function.SetFunction( module, function );
        _register_timer_data.push_back( timer_data );
    }

    void KFTimerModule::AddCountTimer( uint64 object_id, uint64 data_id, uint32 interval_time, uint32 count, KFModule* module, KFTimerFunction& function )
    {
        if ( interval_time == 0u )
        {
            interval_time = 1u;
            __LOG_ERROR__( "module=[{}] id=[{}] interval_time error", module->_plugin_name, object_id );
        }

        auto timer_data = __MAKE_SHARED__( KFTimerData );
        timer_data->_object_id = object_id;
        timer_data->_data_id = data_id;
        timer_data->_type = TimerEnum::Limit;
        timer_data->_count = __MAX__( 1u, count );
        timer_data->_delay = 0u;
        timer_data->_interval = interval_time;
        timer_data->_function.SetFunction( module, function );
        _register_timer_data.push_back( timer_data );
    }

    void KFTimerModule::AddDelayTimer( uint64 object_id, uint64 data_id, uint32 interval_time, KFModule* module, KFTimerFunction& function )
    {
        if ( interval_time == 0u )
        {
            interval_time = 1u;
            __LOG_ERROR__( "module=[{}] id=[{}] interval_time error", module->_plugin_name, object_id );
        }

        // 已经存在就不继续注册
        auto timer_data = FindTimerData( object_id, data_id, module );
        if ( timer_data != nullptr )
        {
            return;
        }

        timer_data = __MAKE_SHARED__( KFTimerData );
        timer_data->_object_id = object_id;
        timer_data->_data_id = data_id;
        timer_data->_type = TimerEnum::Limit;
        timer_data->_count = 1u;
        timer_data->_delay = 0u;
        timer_data->_interval = interval_time;
        timer_data->_function.SetFunction( module, function );
        _register_timer_data.push_back( timer_data );
    }

    void KFTimerModule::RemoveTimer( uint64 object_id, uint64 data_id, KFModule* module )
    {
        auto tuple_data = std::make_tuple( object_id, data_id, module );
        _remove_timer_data.push_back( tuple_data );
        //////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        auto module_timer_data = _module_timer_data_list.Find( module );
        if ( module_timer_data == nullptr )
        {
            return;
        }

        if ( object_id == 0u )
        {
            for ( auto& object_iter : module_timer_data->_object_list._objects )
            {
                auto object_data = object_iter.second;
                for ( auto& timer_iter : object_data->_timer_list._objects )
                {
                    timer_iter.second->_is_prepare_remove = true;
                }
            }
        }
        else if ( data_id == 0 )
        {
            auto object_data = module_timer_data->FindObjectData( object_id );
            if ( object_data != nullptr )
            {
                for ( auto& iter : object_data->_timer_list._objects )
                {
                    iter.second->_is_prepare_remove = true;
                }
            }
        }
        else
        {
            auto timer_data = module_timer_data->FindTimerData( object_id, data_id );
            if ( timer_data != nullptr )
            {
                timer_data->_is_prepare_remove = true;
            }
        }
    }

    void KFTimerModule::RunTimerRemove()
    {
        if ( _remove_timer_data.empty() )
        {
            return;
        }

        for ( auto& tuple_data: _remove_timer_data )
        {
            auto object_id = std::get<0>( tuple_data );
            auto data_id = std::get<1>( tuple_data );
            auto module= std::get<2>( tuple_data );
            RemoveTimerData( object_id, data_id, module );
        }

        _remove_timer_data.clear();
    }

    void KFTimerModule::RunTimerRegister()
    {
        if ( _register_timer_data.empty() )
        {
            return;
        }

        for ( auto timer_data : _register_timer_data )
        {
            // 先删除列表
            RemoveTimerData( timer_data->_object_id, timer_data->_data_id, timer_data->_function.GetModule() );

            // 添加进列表
            AddTimerData( timer_data->_function.GetModule(), timer_data );

            // 加入时间轮中
            AddSlotTimer( timer_data );
        }

        _register_timer_data.clear();
    }

    void KFTimerModule::RunTimerUpdate()
    {
        // 添加总使用时间, 计算时间轮走过的槽数
        _total_run_use_time += KFGlobal::Instance()->_last_frame_use_time;
        auto pass_slot = _total_run_use_time / TimerEnum::SlotTime;
        if ( pass_slot == 0u )
        {
            return;
        }
        _total_run_use_time -= pass_slot * TimerEnum::SlotTime;

        for ( auto i = 0u; i < pass_slot; ++i )
        {
            _now_slot = ( _now_slot + 1u ) % TimerEnum::MaxSlot;
            RunSlotTimer();
        }
    }

    void KFTimerModule::RunSlotTimer()
    {
        std::list<std::shared_ptr<KFTimerData>> done_timer_data_list;

        auto timer_data = _slot_timer_data[ _now_slot ];
        while ( timer_data != nullptr )
        {
            if ( timer_data->_rotation > 0u )
            {
                --timer_data->_rotation;
            }
            else
            {
                done_timer_data_list.push_front( timer_data );
            }

            timer_data = timer_data->_next;
        }

        for ( auto timer_data : done_timer_data_list )
        {
            if ( !timer_data->_is_prepare_remove )
            {
                ExecuteDoneTimer( timer_data );
            }
        }
    }

    void KFTimerModule::ExecuteDoneTimer( std::shared_ptr<KFTimerData> timer_data )
    {
        // 先从列表中删除
        RemoveSlotTimer( timer_data );

        // 执行回调函数
        timer_data->_function.Call( timer_data->_object_id, timer_data->_data_id, ++timer_data->_turns );
        if ( timer_data->_type == TimerEnum::Loop )
        {
            AddSlotTimer( timer_data );
        }
        else if ( timer_data->_type == TimerEnum::Limit )
        {
            --timer_data->_count;
            if ( timer_data->_count == 0u )
            {
                RemoveTimerData( timer_data->_object_id, timer_data->_data_id, timer_data->_function.GetModule() );
            }
            else
            {
                AddSlotTimer( timer_data );
            }
        }
        else
        {
            RemoveTimerData( timer_data->_object_id, timer_data->_data_id, timer_data->_function.GetModule() );
        }
    }
}