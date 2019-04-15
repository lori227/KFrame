#ifndef __KF_SCHEDULE_INTERFACE_H__
#define __KF_SCHEDULE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    enum KFScheduleEnum
    {
        Date = 1,		// 日起
        Week = 2,		// 星期

        ////////////////////////////
        Loop = 1,		// 循环
        Once = 2,		// 一次
    };
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // 计划设定
    class KFScheduleSetting
    {
    public:
        KFScheduleSetting() {}
        virtual ~KFScheduleSetting() {}

        // 设置星期
        virtual void SetDayOfWeek( uint32 controltype, uint32 minute, uint32 hour, uint32 dayofweek, uint32 month = 0 ) = 0;

        // 设置时间
        virtual void SetTime( uint64 time ) = 0;

        // 设置日期
        virtual void SetDate( uint32 controltype, uint32 minute, uint32 hour = 0, uint32 day = 0, uint32 month = 0, uint32 year = 0 ) = 0;

        // 设置回调数据
        virtual void SetData( uint64 objectid, const void* data, uint32 size ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 计划任务回调
    typedef std::function<void( uint64 objectid, const char* data, uint32 size )> KFScheduleFunction;

    class KFScheduleInterface : public KFModule
    {
    public:

        // 创建计划任务设定
        virtual KFScheduleSetting* CreateScheduleSetting() = 0;

        // 注册计划任务
        template< class T >
        void RegisterSchedule( KFScheduleSetting* kfsetting, T* object, void ( T::*handle )( uint64 objectid, const char* data, uint32 size ) )
        {
            KFScheduleFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddSchedule( typeid( T ).name(), kfsetting, function );
        }

        // 删除计划任务
        template< class T >
        void UnRegisterSchedule( T* object )
        {
            RemoveSchedule( typeid( T ).name() );
        }

        // 删除计划任务
        template< class T >
        void UnRegisterSchedule( T* object, uint64 objectid )
        {
            RemoveSchedule( typeid( T ).name(), objectid );
        }

    protected:
        // 注册计划任务
        virtual void AddSchedule( const std::string& module, KFScheduleSetting* kfsetting, KFScheduleFunction& function ) = 0;
        virtual void RemoveSchedule( const std::string& module ) = 0;
        virtual void RemoveSchedule( const std::string& module, uint64 objectid ) = 0;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_schedule, KFScheduleInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __KF_SCHEDULE_FUNCTION__( function )\
    void function( uint64 objectid, const char* data, uint32 size )

#define __REGISTER_SCHEDULE__( kfsetting, function ) \
    _kf_schedule->RegisterSchedule( kfsetting, this, function )

#define  __UNREGISTER_SCHEDULE__() \
    _kf_schedule->UnRegisterSchedule( this )

#define  __UNREGISTER_SCHEDULE_OBJECT__( objectid ) \
    _kf_schedule->UnRegisterSchedule( this, objectid )
}

#endif