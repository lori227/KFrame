#ifndef __KF_SCHEDULE_INTERFACE_H__
#define __KF_SCHEDULE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 计划任务管理, 负责某个时间段内功能的开启和关闭
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 计划任务回调函数,
    typedef std::function<void( uint64 objectid, uint64 durationtime )> KFScheduleFunction;

    class KFScheduleInterface : public KFModule
    {
    public:
        // 注册计划任务
        template< class T >
        void RegisterSchedule( uint32 timeid, uint64 objectid, T* object,
                               void ( T::*starthandle )( uint64, uint64 ),
                               void ( T::*finishhandle )( uint64, uint64 ) )
        {
            KFScheduleFunction startfunction = std::bind( starthandle, object, std::placeholders::_1, std::placeholders::_2 );
            KFScheduleFunction finishfunction = std::bind( finishhandle, object, std::placeholders::_1, std::placeholders::_2 );
            AddSchedule( timeid, typeid( T ).name(), objectid, startfunction, finishfunction );
        }

        // 删除计划任务
        template< class T >
        void UnRegisterSchedule( uint32 timeid, T* object )
        {
            RemoveSchedule( timeid, typeid( T ).name() );
        }

        // 删除计划任务
        template< class T >
        void UnRegisterSchedule( T* object )
        {
            RemoveSchedule( typeid( T ).name() );
        }

    protected:
        // 注册计划任务
        virtual void AddSchedule( uint32 timeid, const std::string& module, uint64 objectid,
                                  KFScheduleFunction& startfunction, KFScheduleFunction& finishfunction ) = 0;
        virtual void RemoveSchedule( const std::string& module ) = 0;
        virtual void RemoveSchedule( uint32 timeid, const std::string& module ) = 0;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_schedule, KFScheduleInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_SCHEDULE_FUNCTION__( function )\
    void function( uint64 objectid, uint64 durationtime )

#define __REGISTER_SCHEDULE__( timeid, objectid, startfunction, finishfunction ) \
    _kf_schedule->RegisterSchedule( timeid, objectid, this, startfunction, finishfunction )

#define  __UN_SCHEDULE_0__() \
    _kf_schedule->UnRegisterSchedule( this )

#define  __UN_SCHEDULE_1__( timeid ) \
    _kf_schedule->UnRegisterSchedule( timeid, this )
}

#endif