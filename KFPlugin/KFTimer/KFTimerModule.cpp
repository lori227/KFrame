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
        for ( auto& iter : _kf_timer_data )
        {
            __KF_DELETE__( KFModuleData, iter.second );
        }
        _kf_timer_data.clear();

        memset( _slot_timer_data, 0u, sizeof( _slot_timer_data ) );
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
    KFTimerData* KFTimerModule::FindTimerData( KFModule* module, uint64 objectid, uint64 subid )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            return nullptr;
        }

        return iter->second->FindTimerData( objectid, subid );
    }

    bool KFTimerModule::AddTimerData( KFModule* module, KFTimerData* kfdata )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            auto moduledata = __KF_NEW__( KFModuleData );
            iter = _kf_timer_data.insert( std::make_pair( module, moduledata ) ).first;
        }

        return iter->second->AddTimerData( kfdata );
    }

    void KFTimerModule::RemoveRegisterData( KFModule* module, uint64 objectid, uint64 subid )
    {
        for ( auto iter = _register_timer_data.begin(); iter != _register_timer_data.end(); )
        {
            auto kfdata = *iter;
            if ( kfdata->_function._module == module &&
                    ( objectid == 0u || kfdata->_object_id == objectid ) &&
                    ( subid == 0u || subid == kfdata->_sub_id ) )
            {
                __KF_DELETE__( KFTimerData, kfdata );
                iter = _register_timer_data.erase( iter );
            }
            else
            {
                ++iter;
            }
        }
    }

    bool KFTimerModule::RemoveTimerData( KFModule* module, uint64 objectid, uint64 subid )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            return false;
        }

        auto moduledata = iter->second;

        // 删除所有
        if ( objectid == 0u )
        {
            for ( auto& siter : moduledata->_object_list )
            {
                auto objectdata = siter.second;
                for ( auto& miter : objectdata->_timer_list )
                {
                    RemoveSlotTimer( miter.second );
                }
            }
            __KF_DELETE__( KFModuleData, moduledata );
            _kf_timer_data.erase( iter );
        }
        else if ( subid == 0 )
        {
            auto objectdata = moduledata->FindObjectData( objectid );
            if ( objectdata != nullptr )
            {
                for ( auto& miter : objectdata->_timer_list )
                {
                    RemoveSlotTimer( miter.second );
                }
                moduledata->RemoveObjectData( objectid );
            }
        }
        else
        {
            auto timerdata = moduledata->FindTimerData( objectid, subid );
            if ( timerdata != nullptr )
            {
                RemoveSlotTimer( timerdata );
                moduledata->RemoveTimerData( objectid, subid );
            }
        }

        return true;
    }

    void KFTimerModule::AddSlotTimer( KFTimerData* timerdata )
    {
        auto ticks = 0u;
        if ( timerdata->_delay != 0u )
        {
            ticks = timerdata->_delay / TimerEnum::SlotTime;
            timerdata->_delay = 0u;
        }
        else
        {
            ticks = timerdata->_interval / TimerEnum::SlotTime;
        }

        // 设置圈数
        timerdata->_rotation = ticks / TimerEnum::MaxSlot;

        // _slot和_now_slot相同时, 会多跑一圈 slot+1 保证下次可以执行
        auto slot = ticks % TimerEnum::MaxSlot;
        timerdata->_slot = ( slot + _now_slot + 1u ) % TimerEnum::MaxSlot;

        auto wheeldata = _slot_timer_data[ timerdata->_slot ];
        if ( wheeldata != nullptr )
        {
            timerdata->_next = wheeldata;
            wheeldata->_prev = timerdata;
        }

        _slot_timer_data[ timerdata->_slot ] = timerdata;
    }

    void KFTimerModule::RemoveSlotTimer( KFTimerData* timerdata )
    {
        auto prevdata = timerdata->_prev;
        if ( prevdata != nullptr )
        {
            prevdata->_next = timerdata->_next;
        }

        auto nextdata = timerdata->_next;
        if ( nextdata != nullptr )
        {
            nextdata->_prev = timerdata->_prev;
        }

        if ( timerdata == _slot_timer_data[ timerdata->_slot ] )
        {
            _slot_timer_data[ timerdata->_slot ] = nextdata;
        }

        timerdata->_prev = nullptr;
        timerdata->_next = nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTimerModule::AddLoopTimer( KFModule* module, uint64 objectid, uint64 subid, uint32 intervaltime, uint32 delaytime, KFTimerFunction& function )
    {
        if ( intervaltime == 0u )
        {
            intervaltime = __MAX_UINT32__;
            __LOG_ERROR__( "module=[{}] id=[{}] intervaltime error", module->_plugin_name, objectid );
        }

        auto kfdata = __KF_NEW__( KFTimerData );
        kfdata->_object_id = objectid;
        kfdata->_sub_id = subid;
        kfdata->_type = TimerEnum::Loop;
        kfdata->_delay = delaytime;
        kfdata->_interval = intervaltime;
        kfdata->_function.SetFunction( module, function );
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::AddLimitTimer( KFModule* module, uint64 objectid, uint64 subid, uint32 intervaltime, uint32 count, KFTimerFunction& function )
    {
        if ( intervaltime == 0u )
        {
            intervaltime = 1u;
            __LOG_ERROR__( "module=[{}] id=[{}] intervaltime error", module->_plugin_name, objectid );
        }

        auto kfdata = __KF_NEW__( KFTimerData );
        kfdata->_object_id = objectid;
        kfdata->_sub_id = subid;
        kfdata->_type = TimerEnum::Limit;
        kfdata->_count = __MAX__( 1u, count );
        kfdata->_delay = 0u;
        kfdata->_interval = intervaltime;
        kfdata->_function.SetFunction( module, function );
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::AddDelayTimer( KFModule* module, uint64 objectid, uint64 subid, uint32 intervaltime, KFTimerFunction& function )
    {
        if ( intervaltime == 0u )
        {
            intervaltime = 1u;
            __LOG_ERROR__( "module=[{}] id=[{}] intervaltime error", module->_plugin_name, objectid );
        }

        // 已经存在就不继续注册
        auto kfdata = FindTimerData( module, objectid, subid );
        if ( kfdata != nullptr )
        {
            return;
        }

        kfdata = __KF_NEW__( KFTimerData );
        kfdata->_object_id = objectid;
        kfdata->_sub_id = subid;
        kfdata->_type = TimerEnum::Limit;
        kfdata->_count = 1u;
        kfdata->_delay = 0u;
        kfdata->_interval = intervaltime;
        kfdata->_function.SetFunction( module, function );
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::RemoveTimer( KFModule* module, uint64 objectid, uint64 subid )
    {
        auto remove = std::make_tuple( module, objectid, subid );
        _remove_timer_data.push_back( remove );
        //////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            return;
        }

        auto moduledata = iter->second;
        if ( objectid == 0u )
        {
            for ( auto& siter : moduledata->_object_list )
            {
                auto objectdata = siter.second;
                for ( auto& miter : objectdata->_timer_list )
                {
                    miter.second->_is_prepare_remove = true;
                }
            }
        }
        else if ( subid == 0 )
        {
            auto objectdata = moduledata->FindObjectData( objectid );
            if ( objectdata != nullptr )
            {
                for ( auto& miter : objectdata->_timer_list )
                {
                    miter.second->_is_prepare_remove = true;
                }
            }
        }
        else
        {
            auto timerdata = moduledata->FindTimerData( objectid, subid );
            if ( timerdata != nullptr )
            {
                timerdata->_is_prepare_remove = true;
            }
        }
    }

    void KFTimerModule::RunTimerRemove()
    {
        if ( _remove_timer_data.empty() )
        {
            return;
        }

        for ( auto& data : _remove_timer_data )
        {
            KFModule* module;
            uint64 objectid = 0u;
            uint64 subid = 0u;
            std::tie( module, objectid, subid ) = data;
            RemoveTimerData( module, objectid, subid );
        }

        _remove_timer_data.clear();
    }

    void KFTimerModule::RunTimerRegister()
    {
        if ( _register_timer_data.empty() )
        {
            return;
        }

        for ( auto kfdata : _register_timer_data )
        {
            // 先删除列表
            RemoveTimerData( kfdata->_function._module, kfdata->_object_id, kfdata->_sub_id );

            // 添加进列表
            AddTimerData( kfdata->_function._module, kfdata );

            // 加入时间轮中
            AddSlotTimer( kfdata );
        }

        _register_timer_data.clear();
    }

    void KFTimerModule::RunTimerUpdate()
    {
        // 添加总使用时间, 计算时间轮走过的槽数
        _total_run_use_time += KFGlobal::Instance()->_last_frame_use_time;
        auto passslot = _total_run_use_time / TimerEnum::SlotTime;
        if ( passslot == 0u )
        {
            return;
        }
        _total_run_use_time -= passslot * TimerEnum::SlotTime;

        for ( auto i = 0u; i < passslot; ++i )
        {
            _now_slot = ( _now_slot + 1u ) % TimerEnum::MaxSlot;
            RunSlotTimer();
        }
    }

    void KFTimerModule::RunSlotTimer()
    {
        std::list< KFTimerData* > donetimerlist;

        auto timerdata = _slot_timer_data[ _now_slot ];
        while ( timerdata != nullptr )
        {
            if ( timerdata->_rotation > 0u )
            {
                --timerdata->_rotation;
            }
            else
            {
                donetimerlist.push_front( timerdata );
            }

            timerdata = timerdata->_next;
        }

        for ( auto timerdata : donetimerlist )
        {
            if ( !timerdata->_is_prepare_remove )
            {
                ExecuteDoneTimer( timerdata );
            }
        }
    }

    void KFTimerModule::ExecuteDoneTimer( KFTimerData* timerdata )
    {
        // 先从列表中删除
        RemoveSlotTimer( timerdata );

        // 执行回调函数
        timerdata->_function.Call( timerdata->_object_id, timerdata->_sub_id );
        if ( timerdata->_type == TimerEnum::Loop )
        {
            AddSlotTimer( timerdata );
        }
        else if ( timerdata->_type == TimerEnum::Limit )
        {
            --timerdata->_count;
            if ( timerdata->_count == 0u )
            {
                RemoveTimerData( timerdata->_function._module, timerdata->_object_id, timerdata->_sub_id );
            }
            else
            {
                AddSlotTimer( timerdata );
            }
        }
        else
        {
            RemoveTimerData( timerdata->_function._module, timerdata->_object_id, timerdata->_sub_id );
        }
    }
}