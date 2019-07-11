#include "KFTimerModule.hpp"

namespace KFrame
{
    KFTimerModule::KFTimerModule()
    {
        _now_slot = 0u;
        _last_update_time = 0u;
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
        _last_update_time = KFGlobal::Instance()->_game_time;
    }

    void KFTimerModule::Run()
    {
        RunTimerRemove();
        RunTimerRegister();
        RunTimerUpdate();
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFTimerData* KFTimerModule::FindTimerData( const std::string& module, uint64 objectid, uint64 subid )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            return nullptr;
        }

        return iter->second->FindTimerData( objectid, subid );
    }

    bool KFTimerModule::AddTimerData( const std::string& module, KFTimerData* kfdata )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            auto moduledata = __KF_NEW__( KFModuleData );
            iter = _kf_timer_data.insert( std::make_pair( module, moduledata ) ).first;
        }

        return iter->second->AddTimerData( kfdata );
    }

    void KFTimerModule::RemoveRegisterData( const std::string& module, uint64 objectid, uint64 subid )
    {
        for ( auto iter = _register_timer_data.begin(); iter != _register_timer_data.end(); )
        {
            auto kfdata = *iter;
            if ( kfdata->_module == module &&
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

    bool KFTimerModule::RemoveTimerData( const std::string& module, uint64 objectid, uint64 subid )
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

    void KFTimerModule::AddSlotTimer( KFTimerData* kfdata )
    {
        auto ticks = 0u;
        if ( kfdata->_delay != 0u )
        {
            ticks = kfdata->_delay / TimerEnum::SlotTime;
            kfdata->_delay = 0u;
        }
        else
        {
            ticks = kfdata->_interval / TimerEnum::SlotTime;
        }

        // 最少需要1个刻度
        ticks = __MAX__( 1u, ticks );

        kfdata->_rotation = ticks / TimerEnum::MaxSlot;
        auto slot = ticks % TimerEnum::MaxSlot;
        kfdata->_slot = ( slot + _now_slot ) % TimerEnum::MaxSlot;

        auto wheeldata = _slot_timer_data[ kfdata->_slot ];
        if ( wheeldata != nullptr )
        {
            kfdata->_next = wheeldata;
            wheeldata->_prev = kfdata;
        }

        _slot_timer_data[ kfdata->_slot ] = kfdata;
    }

    void KFTimerModule::RemoveSlotTimer( KFTimerData* kfdata )
    {
        auto prevdata = kfdata->_prev;
        if ( prevdata != nullptr )
        {
            prevdata->_next = kfdata->_next;
        }

        auto nextdata = kfdata->_next;
        if ( nextdata != nullptr )
        {
            nextdata->_prev = kfdata->_prev;
        }

        if ( kfdata == _slot_timer_data[ kfdata->_slot ] )
        {
            _slot_timer_data[ kfdata->_slot ] = nextdata;
        }

        kfdata->_prev = nullptr;
        kfdata->_next = nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTimerModule::AddLoopTimer( const std::string& module, uint64 objectid, uint64 subid, uint32 intervaltime, uint32 delaytime, KFTimerFunction& function )
    {
        if ( intervaltime == 0u )
        {
            intervaltime = __MAX_UINT32__;
            __LOG_ERROR__( "module=[{}] id=[{}] intervaltime error!", module, objectid );
        }

        auto kfdata = __KF_NEW__( KFTimerData );
        kfdata->_object_id = objectid;
        kfdata->_sub_id = subid;
        kfdata->_module = module;
        kfdata->_type = TimerEnum::Loop;
        kfdata->_function = function;
        kfdata->_interval = intervaltime;
        kfdata->_delay = __MAX__( delaytime, 1u );
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::AddLimitTimer( const std::string& module, uint64 objectid, uint64 subid, uint32 intervaltime, uint32 count, KFTimerFunction& function )
    {
        if ( intervaltime == 0u )
        {
            intervaltime = 1u;
            __LOG_ERROR__( "module=[{}] id=[{}] intervaltime error!", module, objectid );
        }

        auto kfdata = __KF_NEW__( KFTimerData );
        kfdata->_object_id = objectid;
        kfdata->_sub_id = subid;
        kfdata->_module = module;
        kfdata->_type = TimerEnum::Limit;
        kfdata->_count = __MAX__( 1u, count );
        kfdata->_function = function;
        kfdata->_delay = 0u;
        kfdata->_interval = intervaltime;
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::AddDelayTimer( const std::string& module, uint64 objectid, uint64 subid, uint32 intervaltime, KFTimerFunction& function )
    {
        if ( intervaltime == 0u )
        {
            intervaltime = 1u;
            __LOG_ERROR__( "module=[{}] id=[{}] intervaltime error!", module, objectid );
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
        kfdata->_module = module;
        kfdata->_type = TimerEnum::Limit;
        kfdata->_count = 1u;
        kfdata->_function = function;
        kfdata->_delay = 0u;
        kfdata->_interval = intervaltime;
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::RemoveTimer( const std::string& module, uint64 objectid, uint64 subid )
    {
        auto remove = std::make_tuple( module, objectid, subid );
        _remove_timer_data.push_back( remove );
    }

    void KFTimerModule::RunTimerRemove()
    {
        if ( _remove_timer_data.empty() )
        {
            return;
        }

        for ( auto& data : _remove_timer_data )
        {
            std::string module;
            uint64 objectid = 0u;
            uint64 subid = 0u;
            std::tie( module, objectid, subid ) = data;

            RemoveRegisterData( module, objectid, subid );
            RemoveTimerData( module, objectid, subid );
        }

        _remove_timer_data.clear();
    }

    uint32 KFTimerModule::FindLeftTime( const std::string& module, uint64 objectid, uint64 subid )
    {
        auto timerdata = FindTimerData( module, objectid, subid );
        if ( timerdata == nullptr )
        {
            return 0u;
        }

        auto lefttime = timerdata->_rotation * TimerEnum::WheelTime;
        if ( timerdata->_slot >= _now_slot )
        {
            lefttime += ( timerdata->_slot - _now_slot ) * TimerEnum::SlotTime;
        }
        else
        {
            lefttime += ( TimerEnum::MaxSlot + timerdata->_slot - _now_slot ) * TimerEnum::SlotTime;
        }

        return lefttime;
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
            RemoveTimerData( kfdata->_module, kfdata->_object_id, kfdata->_sub_id );

            // 添加进列表
            AddTimerData( kfdata->_module, kfdata );

            // 加入时间轮中
            AddSlotTimer( kfdata );
        }

        _register_timer_data.clear();
    }

    void KFTimerModule::RunTimerUpdate()
    {
        auto nowtime = KFGlobal::Instance()->_game_time;
        auto passslot = ( nowtime - _last_update_time ) / TimerEnum::SlotTime;
        if ( passslot == 0u )
        {
            return;
        }
        _last_update_time += passslot * TimerEnum::SlotTime;

        for ( auto i = 0u; i < passslot; ++i )
        {
            _now_slot = ( _now_slot + 1u ) % TimerEnum::MaxSlot;
            RunSlotTimer();
        }
    }

    void KFTimerModule::RunSlotTimer()
    {
        std::list< KFTimerData* > _done_data;

        auto timerdata = _slot_timer_data[ _now_slot ];
        while ( timerdata != nullptr )
        {
            if ( timerdata->_rotation > 0u )
            {
                --timerdata->_rotation;
            }
            else
            {
                _done_data.push_front( timerdata );
            }

            timerdata = timerdata->_next;
        }

        for ( auto timerdata : _done_data )
        {
            // 先从列表中删除
            RemoveSlotTimer( timerdata );

            // 执行回调函数
            timerdata->_function( timerdata->_module, timerdata->_object_id, timerdata->_sub_id );
            if ( timerdata->_type == TimerEnum::Loop )
            {
                AddSlotTimer( timerdata );
            }
            else if ( timerdata->_type == TimerEnum::Limit )
            {
                --timerdata->_count;
                if ( timerdata->_count == 0u )
                {
                    RemoveTimerData( timerdata->_module, timerdata->_object_id, timerdata->_sub_id );
                }
                else
                {
                    AddSlotTimer( timerdata );
                }
            }
            else
            {
                RemoveTimerData( timerdata->_module, timerdata->_object_id, timerdata->_sub_id );
            }
        }
    }
}