#ifndef __KF_DELAYED_INTERFACE_H__
#define __KF_DELAYED_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 负责处理游戏内的定时功能
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 定时回调函数
    typedef std::function<void( uint64 objectid, const int8* data, uint32 size )> KFDelayedFunction;
    class KFDelayedInterface : public KFModule
    {
    public:
        // 注册定时功能
        template< class T >
        void RegisterDelayed( uint64 time, uint64 objectid, const int8* data, uint32 size,
                              T* object, void ( T::*handle )( uint64, const int8*, uint32 ) )
        {
            KFDelayedFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddDelayedFunction( time, objectid, data, size, typeid( T ).name(), function );
        }

        // 注册定时功能
        template< class T >
        void RegisterDelayed( KFTimeData* timedata, uint64 objectid, const int8* data, uint32 size,
                              T* object, void ( T::*handle )( uint64, const int8*, uint32 ) )
        {
            KFDelayedFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddDelayedFunction( timedata, objectid, data, size, typeid( T ).name(), function );
        }

        // 删除定时功能
        template< class T >
        void UnRegisterDelayed( T* object )
        {
            RemoveDelayedFunction( typeid( T ).name() );
        }

        // 删除计划功能
        template< class T >
        void UnRegisterDelayed( T* object, uint64 objectid )
        {
            RemoveDelayedFunction( typeid( T ).name(), objectid );
        }

    protected:
        virtual void AddDelayedFunction( uint64 time, uint64 objectid, const int8* data, uint32 size, const std::string& module, KFDelayedFunction& function ) = 0;
        virtual void AddDelayedFunction( KFTimeData* timedata, uint64 objectid, const int8* data, uint32 size, const std::string& module, KFDelayedFunction& function ) = 0;
        virtual void RemoveDelayedFunction( const std::string& module ) = 0;
        virtual void RemoveDelayedFunction( const std::string& module, uint64 objectid ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_delayed, KFDelayedInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_DELAYED_FUNCTION__( function )\
    void function( uint64 objectid, const char* data, uint32 size )

#define __REGISTER_DELAYED_NO_DATA__( timedata, objectid, function ) \
    _kf_delayed->RegisterDelayed( timedata, objectid, nullptr, 0, this, function )

#define __REGISTER_DELAYED_WITH_DATA__( timedata, objectid, data, size, function ) \
    _kf_delayed->RegisterDelayed( timedata, objectid, data, size, this, function )

#define  __UN_DELAYED_0__() \
    _kf_delayed->UnRegisterDelayed( this )

#define  __UN_DELAYED_1__( objectid ) \
    _kf_delayed->UnRegisterDelayed( this, objectid )
}

#endif