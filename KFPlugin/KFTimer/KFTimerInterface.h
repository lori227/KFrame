#ifndef __KF_TIMER_INTERFACE_H__
#define __KF_TIMER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function<void( const std::string&, uint64 )> KFTimerFunction;
    ////////////////////////////////////////////////////////////////////////////
    class KFTimerInterface : public KFModule
    {
    public:
        // 注册循环定时器
        template< class T >
        void RegisterLoopTimer( uint64 objectid, uint32 intervaltime, uint32 delaytime, T* object, void ( T::*handle )( const std::string&, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            AddLoopTimer( typeid( T ).name(), objectid, intervaltime, delaytime, function );
        }

        // 注册指定次数定时器
        template< class T >
        void RegisterLimitTimer( uint64 objectid, uint32 intervaltime, uint32 count, T* object, void ( T::*handle )( const std::string&, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            AddLimitTimer( typeid( T ).name(), objectid, intervaltime, count, function );
        }

        // 注册延迟定时器( 一定时间内只执行一次 )
        template< class T >
        void RegisterDelayTimer( uint64 objectid, uint32 intervaltime, T* object, void ( T::*handle )( const std::string&, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            AddDelayTimer( typeid( T ).name(), objectid, intervaltime, function );
        }

        // 删除定时器
        template< class T >
        void UnRegisterTimer( T* object )
        {
            RemoveTimer( typeid( T ).name() );
        }

        template< class T >
        void UnRegisterTimer( T* object, uint64 objectid )
        {
            RemoveTimer( typeid( T ).name(), objectid );
        }

        // 获得定时器剩余时间
        virtual uint32 FindLeftTime( const std::string& module, uint64 objectid ) = 0;

    protected:
        // 注册定时器
        virtual void AddLoopTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 delaytime, KFTimerFunction& function ) = 0;
        virtual void AddLimitTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 count, KFTimerFunction& function ) = 0;
        virtual void AddDelayTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function ) = 0;

        virtual void RemoveTimer( const std::string& module ) = 0;
        virtual void RemoveTimer( const std::string& module, uint64 objectid ) = 0;

    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_timer, KFTimerInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_TIMER_FUNCTION__( function ) \
    void function( const std::string& module, uint64 objectid )

#define __REGISTER_LOOP_TIMER__( objectid, intervaltime, delaytime, function ) \
    _kf_timer->RegisterLoopTimer( objectid, intervaltime, delaytime, this, function )

#define __REGISTER_LIMIT_TIMER__( objectid, intervaltime, count, function ) \
    _kf_timer->RegisterLimitTimer( objectid, intervaltime, count, this, function )

#define __REGISTER_DELAY_TIMER__( objectid, intervaltime, function ) \
    _kf_timer->RegisterDelayTimer( objectid, intervaltime, this, function )

#define __UNREGISTER_TIMER__() \
    _kf_timer->UnRegisterTimer( this )

#define __UNREGISTER_OBJECT_TIMER__( objectid ) \
    _kf_timer->UnRegisterTimer( this, objectid )

}



#endif