#ifndef __KF_TIMER_INTERFACE_H__
#define __KF_TIMER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function<void( const std::string&, uint64, uint64 )> KFTimerFunction;
    ////////////////////////////////////////////////////////////////////////////
    class KFTimerInterface : public KFModule
    {
    public:
        // 注册循环定时器
        template< class T >
        void RegisterLoopTimer( uint64 objectid, uint64 subid, uint32 intervaltime, uint32 delaytime, T* object, void ( T::*handle )( const std::string&, uint64, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddLoopTimer( typeid( T ).name(), objectid, subid, intervaltime, delaytime, function );
        }

        // 注册指定次数定时器
        template< class T >
        void RegisterLimitTimer( uint64 objectid, uint64 subid, uint32 intervaltime, uint32 count, T* object, void ( T::*handle )( const std::string&, uint64, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddLimitTimer( typeid( T ).name(), objectid, subid, intervaltime, count, function );
        }

        // 注册延迟定时器( 一定时间内只执行一次 )
        template< class T >
        void RegisterDelayTimer( uint64 objectid, uint64 subid, uint32 intervaltime, T* object, void ( T::*handle )( const std::string&, uint64, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddDelayTimer( typeid( T ).name(), objectid, subid, intervaltime, function );
        }

        // 删除定时器
        template< class T >
        void UnRegisterTimer( T* object, uint64 objectid, uint64 subid )
        {
            RemoveTimer( typeid( T ).name(), objectid, subid );
        }

        // 获得定时器剩余时间
        virtual uint32 FindLeftTime( const std::string& module, uint64 objectid, uint64 subid ) = 0;

    protected:
        // 注册定时器
        virtual void AddLoopTimer( const std::string& module, uint64 objectid, uint64 subid, uint32 intervaltime, uint32 delaytime, KFTimerFunction& function ) = 0;
        virtual void AddLimitTimer( const std::string& module, uint64 objectid, uint64 subid, uint32 intervaltime, uint32 count, KFTimerFunction& function ) = 0;
        virtual void AddDelayTimer( const std::string& module, uint64 objectid, uint64 subid, uint32 intervaltime, KFTimerFunction& function ) = 0;
        virtual void RemoveTimer( const std::string& module, uint64 objectid, uint64 subid ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_timer, KFTimerInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_TIMER_FUNCTION__( function ) \
    void function( const std::string& module, uint64 objectid, uint64 subid )

#define __LOOP_TIMER_0__( intervaltime, delaytime, function ) \
    _kf_timer->RegisterLoopTimer( 0u, 0u, intervaltime, delaytime, this, function )
#define __LOOP_TIMER_1__( objectid, intervaltime, delaytime, function ) \
    _kf_timer->RegisterLoopTimer( objectid, 0u, intervaltime, delaytime, this, function )
#define __LOOP_TIMER_2__( objectid, subid, intervaltime, delaytime, function ) \
    _kf_timer->RegisterLoopTimer( objectid, subid, intervaltime, delaytime, this, function )

#define __LIMIT_TIMER_0__( intervaltime, count, function ) \
    _kf_timer->RegisterLimitTimer( 0u, 0u, intervaltime, count, this, function )
#define __LIMIT_TIMER_1__( objectid, intervaltime, count, function ) \
    _kf_timer->RegisterLimitTimer( objectid, 0u, intervaltime, count, this, function )
#define __LIMIT_TIMER_2__( objectid, subid, intervaltime, count, function ) \
    _kf_timer->RegisterLimitTimer( objectid, subid, intervaltime, count, this, function )

#define __DELAY_TIMER_0__( intervaltime, function ) \
    _kf_timer->RegisterDelayTimer( 0u, 0u,intervaltime, this, function )
#define __DELAY_TIMER_1__( objectid, intervaltime, function ) \
    _kf_timer->RegisterDelayTimer( objectid, 0u,intervaltime, this, function )
#define __DELAY_TIMER_2__( objectid, subid, intervaltime, function ) \
    _kf_timer->RegisterDelayTimer( objectid, subid, intervaltime, this, function )

#define __UN_TIMER_0__() \
    _kf_timer->UnRegisterTimer( this, 0u, 0u )
#define __UN_TIMER_1__( objectid ) \
    _kf_timer->UnRegisterTimer( this, objectid, 0u )
#define __UN_TIMER_2__( objectid, subid ) \
    _kf_timer->UnRegisterTimer( this, objectid, subid )
}

#endif