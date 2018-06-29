#ifndef __KF_TIMER_INTERFACE_H__
#define __KF_TIMER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function<void( const std::string&, uint64 )> KFTimerFunction;
#define __KF_TIMER_FUNCTION__( function ) void function( const std::string& module, uint64 objectid )
    ////////////////////////////////////////////////////////////////////////////
    class KFTimerInterface : public KFModule
    {
    public:
        template< class T >
        void RegisterLoopTimer( uint64 objectid, uint32 intervaltime, T* object,
                                void ( T::*handle )( const std::string&, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object,
                                                  std::placeholders::_1, std::placeholders::_2 );
            AddLoopTimer( typeid( T ).name(), objectid, intervaltime, function );
        }

        template< class T >
        void RegisterLimitTimer( uint64 objectid, uint32 intervaltime, uint32 count, T* object,
                                 void ( T::*handle )( const std::string&, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object,
                                                  std::placeholders::_1, std::placeholders::_2 );
            AddLimitTimer( typeid( T ).name(), objectid, intervaltime, count, function );
        }

        template< class T >
        void RegisterDelayTimer( uint64 objectid, uint32 intervaltime, T* object,
                                 void ( T::*handle )( const std::string&, uint64 ) )
        {
            KFTimerFunction function = std::bind( handle, object,
                                                  std::placeholders::_1, std::placeholders::_2 );
            AddDelayTimer( typeid( T ).name(), objectid, intervaltime, function );
        }

        // 删除定时器
        template< class T >
        bool UnRegisterTimer( T* object )
        {
            return RemoveTimer( typeid( T ).name() );
        }

        template< class T >
        bool UnRegisterTimer( T* object, uint64 objectid )
        {
            return RemoveTimer( typeid( T ).name(), objectid );
        }

        // 获得定时器剩余时间
        virtual uint32 FindLeftTime( const std::string& module, uint64 objectid ) = 0;

    protected:
        // 注册定时器
        virtual void AddLoopTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function ) = 0;
        virtual void AddLimitTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 count, KFTimerFunction& function ) = 0;
        virtual void AddDelayTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function ) = 0;

        virtual bool RemoveTimer( const std::string& module ) = 0;
        virtual bool RemoveTimer( const std::string& module, uint64 objectid ) = 0;

    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_timer, KFTimerInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif