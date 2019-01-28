#include "KFTimerModule.hpp"

namespace KFrame
{
    KFTimerModule::KFTimerModule()
    {
        _now_slot = 0;
        _last_update_time = 0;
        memset( _slot_timer_data, 0, sizeof( _slot_timer_data ) );
    }

    KFTimerModule::~KFTimerModule()
    {

    }

    void KFTimerModule::BeforeRun()
    {
        _now_slot = 0;
        _last_update_time = KFGlobal::Instance()->_game_time;
    }

    void KFTimerModule::ShutDown()
    {
        for ( auto iter : _kf_timer_data )
        {
            for ( auto miter : iter.second )
            {
                __KF_DELETE__( KFTimerData, miter.second );
            }
        }

        _kf_timer_data.clear();
        memset( _slot_timer_data, 0, sizeof( _slot_timer_data ) );
    }

    void KFTimerModule::Run()
    {
        RunTimerRemove();
        RunTimerRegister();
        RunTimerUpdate();
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFTimerData* KFTimerModule::FindTimerData( const std::string& module, uint64 objectid )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            return nullptr;
        }

        auto miter = iter->second.find( objectid );
        if ( miter == iter->second.end() )
        {
            return nullptr;
        }

        return miter->second;
    }

    bool KFTimerModule::AddTimerData( const std::string& module, uint64 objectid, KFTimerData* kfdata )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            std::map< uint64, KFTimerData* > temp;
            iter = _kf_timer_data.insert( std::make_pair( module, temp ) ).first;
        }

        return iter->second.insert( std::make_pair( objectid, kfdata ) ).second;
    }

    void KFTimerModule::RemoveRegisterData( const std::string& module, uint64 objectid )
    {
        for ( auto iter = _register_timer_data.begin(); iter != _register_timer_data.end(); )
        {
            auto kfdata = *iter;
            if ( kfdata->_module == module &&
                    ( objectid == _invalid_int || kfdata->_id == objectid ) )
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

    bool KFTimerModule::RemoveTimerData( const std::string& module )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            return false;
        }

        for ( auto miter : iter->second )
        {
            auto kfdata = miter.second;

            RemoveSlotTimer( kfdata );
            __KF_DELETE__( KFTimerData, kfdata );
        }
        iter->second.clear();

        _kf_timer_data.erase( iter );

        return true;
    }

    bool KFTimerModule::RemoveTimerData( const std::string& module, uint64 objectid )
    {
        auto iter = _kf_timer_data.find( module );
        if ( iter == _kf_timer_data.end() )
        {
            return false;
        }

        auto miter = iter->second.find( objectid );
        if ( miter == iter->second.end() )
        {
            return false;
        }

        auto kfdata = miter->second;
        RemoveSlotTimer( kfdata );
        __KF_DELETE__( KFTimerData, kfdata );

        iter->second.erase( miter );
        if ( iter->second.empty() )
        {
            _kf_timer_data.erase( iter );
        }

        return true;
    }

    void KFTimerModule::AddSlotTimer( KFTimerData* kfdata, bool firstadd )
    {
        if ( firstadd )
        {
            kfdata->_rotation = 0;
            kfdata->_slot = ( _now_slot + 1 ) % TimerEnum::MaxSlot;
        }
        else
        {
            uint32 ticks = kfdata->_interval / TimerEnum::SlotTime;

            kfdata->_rotation = ticks / TimerEnum::MaxSlot;
            kfdata->_slot = ( ( ticks % TimerEnum::MaxSlot ) + _now_slot ) % TimerEnum::MaxSlot;
        }

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
    void KFTimerModule::AddLoopTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function )
    {
        auto kfdata = __KF_NEW__( KFTimerData );
        kfdata->_id = objectid;
        kfdata->_module = module;
        kfdata->_type = TimerEnum::Loop;
        kfdata->_function = function;
        kfdata->_interval = intervaltime;
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::AddLimitTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 count, KFTimerFunction& function )
    {
        auto kfdata = __KF_NEW__( KFTimerData );
        kfdata->_id = objectid;
        kfdata->_module = module;
        kfdata->_type = TimerEnum::Limit;
        kfdata->_count = __MAX__( 1, count );
        kfdata->_function = function;
        kfdata->_interval = intervaltime;
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::AddDelayTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function )
    {
        // 已经存在就不继续注册
        auto kfdata = FindTimerData( module, objectid );
        if ( kfdata != nullptr )
        {
            return;
        }

        kfdata = __KF_NEW__( KFTimerData );
        kfdata->_id = objectid;
        kfdata->_module = module;
        kfdata->_type = TimerEnum::Limit;
        kfdata->_count = 1;
        kfdata->_function = function;
        kfdata->_interval = intervaltime;
        _register_timer_data.push_back( kfdata );
    }

    void KFTimerModule::RemoveTimer( const std::string& module )
    {
        _remove_timer_data[ module ] = _invalid_int;
    }

    void KFTimerModule::RemoveTimer( const std::string& module, uint64 objectid )
    {
        _remove_timer_data[ module ] = objectid;
    }

    void KFTimerModule::RunTimerRemove()
    {
        if ( _remove_timer_data.empty() )
        {
            return;
        }

        for ( auto& iter : _remove_timer_data )
        {
            auto& module = iter.first;
            auto objectid = iter.second;

            RemoveRegisterData( module, objectid );
            if ( objectid == _invalid_int )
            {
                RemoveTimerData( module );
            }
            else
            {
                RemoveTimerData( module, objectid );
            }
        }

        _remove_timer_data.clear();
    }

    uint32 KFTimerModule::FindLeftTime( const std::string& module, uint64 objectid )
    {
        auto timerdata = FindTimerData( module, objectid );
        if ( timerdata == nullptr )
        {
            return 0;
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
            RemoveTimerData( kfdata->_module, kfdata->_id );

            // 添加进列表
            AddTimerData( kfdata->_module, kfdata->_id, kfdata );

            // 加入时间轮中
            if ( kfdata->_type == TimerEnum::Loop )
            {
                AddSlotTimer( kfdata, true );
            }
            else
            {
                AddSlotTimer( kfdata, false );
            }
        }

        _register_timer_data.clear();
    }

    void KFTimerModule::RunTimerUpdate()
    {
        auto nowtime = KFGlobal::Instance()->_game_time;
        auto passslot = ( nowtime - _last_update_time ) / TimerEnum::SlotTime;
        if ( passslot == 0 )
        {
            return;
        }
        _last_update_time += passslot * TimerEnum::SlotTime;

        for ( auto i = 0u; i < passslot; ++i )
        {
            _now_slot = ( _now_slot + 1 ) % TimerEnum::MaxSlot;
            RunSlotTimer();
        }
    }

    void KFTimerModule::RunSlotTimer()
    {
        std::list< KFTimerData* > _done_data;

        auto timerdata = _slot_timer_data[ _now_slot ];
        while ( timerdata != nullptr )
        {
            if ( timerdata->_rotation > 0 )
            {
                --timerdata->_rotation;
            }

            if ( timerdata->_rotation == 0 )
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
            timerdata->_function( timerdata->_module, timerdata->_id );
            if ( timerdata->_type == TimerEnum::Loop )
            {
                AddSlotTimer( timerdata, false );
            }
            else if ( timerdata->_type == TimerEnum::Limit )
            {
                --timerdata->_count;
                if ( timerdata->_count == 0 )
                {
                    RemoveTimerData( timerdata->_module, timerdata->_id );
                }
                else
                {
                    AddSlotTimer( timerdata, false );
                }
            }
            else
            {
                RemoveTimerData( timerdata->_module, timerdata->_id );
            }
        }
    }
}