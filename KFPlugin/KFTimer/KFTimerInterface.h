#ifndef __KF_TIMER_INTERFACE_H__
#define __KF_TIMER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function<void( uint64, uint64, uint32 )> KFTimerFunction;
    ////////////////////////////////////////////////////////////////////////////
    class KFTimerInterface : public KFModule
    {
    public:
        // 注册循环定时器
        template<class T>
        void RegisterLoopTimer( uint64 object_id, uint64 data_id, uint32 interval_time, uint32 delay_time, T* module, void ( T::*handle )( uint64, uint64, uint32 ) )
        {
            KFTimerFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddLoopTimer( object_id, data_id, interval_time, delay_time, module, function );
        }

        // 注册指定次数定时器
        template<class T>
        void RegisterCountTimer( uint64 object_id, uint64 data_id, uint32 interval_time, uint32 count, T* module, void ( T::*handle )( uint64, uint64, uint32 ) )
        {
            KFTimerFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddCountTimer( object_id, data_id, interval_time, count, module, function );
        }

        // 注册延迟定时器( 一定时间内只执行一次 )
        template<class T>
        void RegisterDelayTimer( uint64 object_id, uint64 data_id, uint32 interval_time, T* module, void ( T::*handle )( uint64, uint64, uint32 ) )
        {
            KFTimerFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddDelayTimer( object_id, data_id, interval_time, module, function );
        }

        // 删除定时器
        template<class T>
        void UnRegisterTimer( T* module, uint64 object_id, uint64 data_id )
        {
            RemoveTimer( object_id, data_id, module );
        }

    protected:
        // 注册定时器
        virtual void AddLoopTimer( uint64 object_id, uint64 data_id, uint32 interval_time, uint32 delay_time, KFModule* module, KFTimerFunction& function ) = 0;
        virtual void AddCountTimer( uint64 object_id, uint64 data_id, uint32 interval_time, uint32 count, KFModule* module, KFTimerFunction& function ) = 0;
        virtual void AddDelayTimer( uint64 object_id, uint64 data_id, uint32 interval_time, KFModule* module, KFTimerFunction& function ) = 0;
        virtual void RemoveTimer( uint64 object_id, uint64 data_id, KFModule* module ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_timer, KFTimerInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_TIMER_FUNCTION__( function ) \
    void function( uint64 object_id, uint64 data_id, uint32 turns )

#define __LOOP_TIMER_0__( interval_time, delay_time, function ) \
    _kf_timer->RegisterLoopTimer( 0u, 0u, interval_time, delay_time, this, function )
#define __LOOP_TIMER_1__( object_id, interval_time, delay_time, function ) \
    _kf_timer->RegisterLoopTimer( object_id, 0u, interval_time, delay_time, this, function )
#define __LOOP_TIMER_2__( object_id, data_id, interval_time, delay_time, function ) \
    _kf_timer->RegisterLoopTimer( object_id, data_id, interval_time, delay_time, this, function )

#define __COUNT_TIMER_0__( interval_time, count, function ) \
    _kf_timer->RegisterCountTimer( 0u, 0u, interval_time, count, this, function )
#define __COUNT_TIMER_1__( object_id, interval_time, count, function ) \
    _kf_timer->RegisterCountTimer( object_id, 0u, interval_time, count, this, function )
#define __COUNT_TIMER_2__( object_id, data_id, interval_time, count, function ) \
    _kf_timer->RegisterCountTimer( object_id, data_id, interval_time, count, this, function )

#define __DELAY_TIMER_0__( interval_time, function ) \
    _kf_timer->RegisterDelayTimer( 0u, 0u,interval_time, this, function )
#define __DELAY_TIMER_1__( object_id, interval_time, function ) \
    _kf_timer->RegisterDelayTimer( object_id, 0u,interval_time, this, function )
#define __DELAY_TIMER_2__( object_id, data_id, interval_time, function ) \
    _kf_timer->RegisterDelayTimer( object_id, data_id, interval_time, this, function )

#define __UN_TIMER_0__() \
    _kf_timer->UnRegisterTimer( this, 0u, 0u )
#define __UN_TIMER_1__( object_id ) \
    _kf_timer->UnRegisterTimer( this, object_id, 0u )
#define __UN_TIMER_2__( object_id, data_id ) \
    _kf_timer->UnRegisterTimer( this, object_id, data_id )
}

#endif