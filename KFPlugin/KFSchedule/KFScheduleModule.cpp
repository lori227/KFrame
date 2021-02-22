#include "KFScheduleModule.hpp"

namespace KFrame
{
    KFScheduleModule::~KFScheduleModule()
    {
        for ( auto data : _kf_schedule_register )
        {
            __KF_DELETE__( KFScheduleData, data );
        }

        _kf_schedule_register.clear();
    }

    void KFScheduleModule::Run()
    {
        RunScheduleRemove();
        RunScheduleRegister();
        RunScheduleUpdate();
    }

    void KFScheduleModule::BeforeRun()
    {
        CalcNextCheckRunTime();
    }

    void KFScheduleModule::CalcNextCheckRunTime()
    {
        auto minute = KFGlobal::Instance()->_real_time / KFTimeEnum::OneMinuteSecond;
        _next_check_run_time = ( minute + 1u ) * KFTimeEnum::OneMinuteSecond;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFScheduleModule::AddSchedule( uint32 timesectionid, KFModule* module, uint64 objectid,
                                        KFScheduleFunction& startfunction,
                                        KFScheduleFunction& finishfunction )
    {
        auto scheduledata = __KF_NEW__( KFScheduleData );
        scheduledata->_time_section_id = timesectionid;
        scheduledata->_object_id = objectid;
        scheduledata->_start_function.SetFunction( module, startfunction );
        scheduledata->_finish_function.SetFunction( module, finishfunction );
        _kf_schedule_register.push_back( scheduledata );
    }

    void KFScheduleModule::RemoveSchedule( KFModule* module )
    {
        for ( auto& iter : _kf_schedule_list._objects )
        {
            for ( auto& miter : iter.second->_schedule_data_list._objects )
            {
                auto scheduledata = miter.second;
                if ( scheduledata->_start_function._module == module )
                {
                    _kf_schedule_remove.emplace_back( std::make_tuple( scheduledata->_time_section_id, scheduledata->_start_function._module ) );
                }
            }
        }
    }

    void KFScheduleModule::RemoveSchedule( uint32 timeid, KFModule* module )
    {
        _kf_schedule_remove.emplace_back( std::make_tuple( timeid, module ) );
    }

    void KFScheduleModule::RunScheduleRemove()
    {
        // 删除无效和执行过的任务
        if ( _kf_schedule_remove.empty() )
        {
            return;
        }

        for ( auto& data : _kf_schedule_remove )
        {
            auto _time_section_id = std::get<0>( data );
            auto module = std::get<1>( data );

            auto kfschedulelist = _kf_schedule_list.Find( _time_section_id );
            if ( kfschedulelist != nullptr )
            {
                kfschedulelist->_schedule_data_list.Remove( module );
                if ( kfschedulelist->_schedule_data_list.IsEmpty() )
                {
                    _kf_schedule_list.Remove( _time_section_id );
                }
            }
        }

        _kf_schedule_remove.clear();
    }

    void KFScheduleModule::RunScheduleRegister()
    {
        if ( _kf_schedule_register.empty() )
        {
            return;
        }

        for ( auto kfdata : _kf_schedule_register )
        {
            AddSchedule( kfdata );
        }

        _kf_schedule_register.clear();
    }

    void KFScheduleModule::AddSchedule( KFScheduleData* scheduledata )
    {
        auto schedulelist = _kf_schedule_list.Create( scheduledata->_time_section_id );
        schedulelist->_time_section_id = scheduledata->_time_section_id;
        schedulelist->_schedule_data_list.Insert( scheduledata->_start_function._module, scheduledata );

        // 如果正在执行, 需要执行回调
        if ( schedulelist->_status == KFScheduleEnum::Runing )
        {
            scheduledata->_start_function.Call( scheduledata->_object_id, schedulelist->_duration_time );
        }
    }

    void KFScheduleModule::RunScheduleUpdate()
    {
        // 还没有到时间
        if ( _next_check_run_time > KFGlobal::Instance()->_real_time )
        {
            return;
        }
        CalcNextCheckRunTime();

        // 检查计划任务状态
        KFDate now_date( KFGlobal::Instance()->_real_time );
        for ( auto& iter : _kf_schedule_list._objects )
        {
            auto schedulelist = iter.second;
            switch ( schedulelist->_status )
            {
            case KFScheduleEnum::Stop:
                ExecuteScheduleStart( schedulelist, now_date );
                break;
            case KFScheduleEnum::Runing:
                ExecuteScheduleFinish( schedulelist, now_date );
                break;
            default:
                break;
            }
        }
    }

    void KFScheduleModule::ExecuteScheduleStart( KFScheduleDataList* schedulelist, KFDate& now_date )
    {
        auto kftimesetting = KFTimeSectionConfig::Instance()->FindSetting( schedulelist->_time_section_id );
        if ( kftimesetting == nullptr )
        {
            return;
        }

        for ( auto& time_section : kftimesetting->_time_section )
        {
            // 在[starttime,endtime]区间内
            if ( KFDate::CheckInTimeSection( &time_section, now_date ) )
            {
                schedulelist->_status = KFScheduleEnum::Runing;
                schedulelist->_finish_time_data = time_section._finish_time_data;
                //schedulelist->_duration_time = CaclSectionTimeDataDuration( &time_section._start_time, &time_section._end_time );

                // 功能函数回调
                for ( auto& iter : schedulelist->_schedule_data_list._objects )
                {
                    auto scheduledata = iter.second;
                    scheduledata->_start_function.Call( scheduledata->_object_id, schedulelist->_duration_time );
                }
            }

            break;
        }
    }

    void KFScheduleModule::ExecuteScheduleFinish( KFScheduleDataList* schedulelist, KFDate& now_date )
    {
        // 没有过结束时间
        if ( !KFDate::CheckSectionTimeData( &schedulelist->_finish_time_data, now_date ) )
        {
            return;
        }

        schedulelist->_status = KFScheduleEnum::Stop;

        // 功能函数回调
        for ( auto& iter : schedulelist->_schedule_data_list._objects )
        {
            auto scheduledata = iter.second;
            scheduledata->_finish_function.Call( scheduledata->_object_id, schedulelist->_duration_time );
        }
    }
}