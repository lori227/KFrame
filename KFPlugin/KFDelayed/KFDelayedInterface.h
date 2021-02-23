#ifndef __KF_DELAYED_INTERFACE_H__
#define __KF_DELAYED_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 负责处理游戏内的定时功能
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 定时回调函数
    typedef std::function<void( uint64 object_id, const int8* data, uint32 size )> KFDelayedFunction;
    class KFDelayedInterface : public KFModule
    {
    public:
        // 注册定时功能
        template<class T>
        void RegisterDelayed( uint64 time, uint64 object_id, const int8* data, uint32 size,
                              T* module, void ( T::*handle )( uint64, const int8*, uint32 ) )
        {
            KFDelayedFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddDelayedFunction( time, object_id, data, size, module, function );
        }

        // 注册定时功能
        template<class T>
        void RegisterDelayed( KFTimeData* time_data, uint64 object_id, const int8* data, uint32 size,
                              T* module, void ( T::*handle )( uint64, const int8*, uint32 ) )
        {
            KFDelayedFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddDelayedFunction( time_data, object_id, data, size, module, function );
        }

        // 删除定时功能
        template<class T>
        void UnRegisterDelayed( T* module )
        {
            RemoveDelayedFunction( module );
        }

        // 删除计划功能
        template<class T>
        void UnRegisterDelayed( T* module, uint64 object_id )
        {
            RemoveDelayedFunction( object_id, module );
        }

    protected:
        virtual void AddDelayedFunction( uint64 time, uint64 object_id, const int8* data, uint32 size, KFModule* module, KFDelayedFunction& function ) = 0;
        virtual void AddDelayedFunction( KFTimeData* time_data, uint64 object_id, const int8* data, uint32 size, KFModule* module, KFDelayedFunction& function ) = 0;
        virtual void RemoveDelayedFunction( KFModule* module ) = 0;
        virtual void RemoveDelayedFunction( uint64 object_id, KFModule* module ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_delayed, KFDelayedInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_DELAYED_FUNCTION__( function )\
    void function( uint64 object_id, const char* data, uint32 size )

#define __REGISTER_DELAYED_NO_DATA__( time_data, object_id, function ) \
    _kf_delayed->RegisterDelayed( time_data, object_id, nullptr, 0, this, function )

#define __REGISTER_DELAYED_WITH_DATA__( time_data, object_id, data, size, function ) \
    _kf_delayed->RegisterDelayed( time_data, object_id, data, size, this, function )

#define  __UN_DELAYED_0__() \
    _kf_delayed->UnRegisterDelayed( this )

#define  __UN_DELAYED_1__( object_id ) \
    _kf_delayed->UnRegisterDelayed( this, object_id )
}

#endif