#ifndef __KF_SCHEDULE_INTERFACE_H__
#define __KF_SCHEDULE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 计划任务管理, 负责某个时间段内功能的开启和关闭
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 计划任务回调函数,
    typedef std::function<void( uint64 object_id, uint64 duration_time )> KFScheduleFunction;

    class KFScheduleInterface : public KFModule
    {
    public:
        // 注册计划任务
        template<class T>
        void RegisterSchedule( uint32 time_section_id, uint64 object_id, T* module,
                               void ( T::*start_handle )( uint64, uint64 ),
                               void ( T::*finish_handle )( uint64, uint64 ) )
        {
            KFScheduleFunction start_function = std::bind( start_handle, module, std::placeholders::_1, std::placeholders::_2 );
            KFScheduleFunction finish_function = std::bind( finish_handle, module, std::placeholders::_1, std::placeholders::_2 );
            AddSchedule( time_section_id, object_id, module, start_function, finish_function );
        }

        // 删除计划任务
        template<class T>
        void UnRegisterSchedule( uint32 time_section_id, T* module )
        {
            RemoveSchedule( time_section_id, module );
        }

        // 删除计划任务
        template<class T>
        void UnRegisterSchedule( T* module )
        {
            RemoveSchedule( module );
        }

    protected:
        // 注册计划任务
        virtual void AddSchedule( uint32 time_section_id, uint64 object_id, KFModule* module,
                                  KFScheduleFunction& start_function, KFScheduleFunction& finish_function ) = 0;
        virtual void RemoveSchedule( KFModule* module ) = 0;
        virtual void RemoveSchedule( uint32 time_section_id, KFModule* module ) = 0;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_schedule, KFScheduleInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_SCHEDULE_FUNCTION__( function )\
    void function( uint64 object_id, uint64 duration_time )

#define __REGISTER_SCHEDULE__( time_section_id, object_id, start_function, finish_function ) \
    _kf_schedule->RegisterSchedule( time_section_id, object_id, this, start_function, finish_function )

#define  __UN_SCHEDULE_0__() \
    _kf_schedule->UnRegisterSchedule( this )

#define  __UN_SCHEDULE_1__( time_section_id ) \
    _kf_schedule->UnRegisterSchedule( time_section_id, this )
}

#endif