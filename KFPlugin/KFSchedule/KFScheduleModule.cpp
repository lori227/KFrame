#include "KFScheduleModule.hpp"

namespace KFrame
{
    KFScheduleModule::~KFScheduleModule()
    {
        _schedule_run_list.Clear();
        _schedule_register_list.clear();
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
    void KFScheduleModule::AddSchedule( uint32 time_section_id, uint64 object_id, KFModule* module,
                                        KFScheduleFunction& start_function,
                                        KFScheduleFunction& finish_function )
    {
        auto schedule_data = __MAKE_SHARED__( KFScheduleData );
        schedule_data->_time_section_id = time_section_id;
        schedule_data->_object_id = object_id;
        schedule_data->_start_function.SetFunction( module, start_function );
        schedule_data->_finish_function.SetFunction( module, finish_function );
        _schedule_register_list.push_back( schedule_data );
    }

    void KFScheduleModule::RemoveSchedule( KFModule* module )
    {
        for ( auto& iter : _schedule_run_list._objects )
        {
            for ( auto& miter : iter.second->_schedule_data_list._objects )
            {
                auto schedule_data = miter.second;
                if ( schedule_data->_start_function.GetModule() == module )
                {
                    _schedule_remove_list.emplace_back( std::make_tuple( schedule_data->_time_section_id, schedule_data->_start_function.GetModule() ) );
                }
            }
        }
    }

    void KFScheduleModule::RemoveSchedule( uint32 time_section_id, KFModule* module )
    {
        _schedule_remove_list.emplace_back( std::make_tuple( time_section_id, module ) );
    }

    void KFScheduleModule::RunScheduleRemove()
    {
        // 删除无效和执行过的任务
        if ( _schedule_remove_list.empty() )
        {
            return;
        }

        for ( auto& data : _schedule_remove_list )
        {
            auto time_section_id = std::get<0>( data );
            auto module = std::get<1>( data );

            auto schedule_list = _schedule_run_list.Find( time_section_id );
            if ( schedule_list != nullptr )
            {
                schedule_list->_schedule_data_list.Remove( module );
                if ( schedule_list->_schedule_data_list.IsEmpty() )
                {
                    _schedule_run_list.Remove( time_section_id );
                }
            }
        }

        _schedule_remove_list.clear();
    }

    void KFScheduleModule::RunScheduleRegister()
    {
        if ( _schedule_register_list.empty() )
        {
            return;
        }

        for ( auto schedule_data : _schedule_register_list )
        {
            AddSchedule( schedule_data );
        }

        _schedule_register_list.clear();
    }

    void KFScheduleModule::AddSchedule( std::shared_ptr<KFScheduleData> schedule_data )
    {
        auto schedule_list = _schedule_run_list.Create( schedule_data->_time_section_id );
        schedule_list->_time_section_id = schedule_data->_time_section_id;
        schedule_list->_schedule_data_list.Insert( schedule_data->_start_function.GetModule(), schedule_data );

        // 如果正在执行, 需要执行回调
        if ( schedule_list->_status == KFScheduleEnum::Running )
        {
            schedule_data->_start_function.Call( schedule_data->_object_id, schedule_list->_duration_time );
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
        for ( auto& iter : _schedule_run_list._objects )
        {
            auto schedule_list = iter.second;
            switch ( schedule_list->_status )
            {
            case KFScheduleEnum::Stop:
                ExecuteScheduleStart( schedule_list, now_date );
                break;
            case KFScheduleEnum::Running:
                ExecuteScheduleFinish( schedule_list, now_date );
                break;
            default:
                break;
            }
        }
    }

    void KFScheduleModule::ExecuteScheduleStart( std::shared_ptr<KFScheduleDataList> schedule_list, KFDate& now_date )
    {
        auto time_section_setting = KFTimeSectionConfig::Instance()->FindSetting( schedule_list->_time_section_id );
        if ( time_section_setting == nullptr )
        {
            return;
        }

        for ( auto& time_section : time_section_setting->_time_section )
        {
            // 在[starttime,endtime]区间内
            if ( KFDate::CheckInTimeSection( &time_section, now_date ) )
            {
                schedule_list->_status = KFScheduleEnum::Running;
                schedule_list->_finish_time_data = time_section._finish_time_data;
                //schedule_list->_duration_time = CaclSectionTimeDataDuration( &time_section._start_time, &time_section._end_time );

                // 功能函数回调
                for ( auto& iter : schedule_list->_schedule_data_list._objects )
                {
                    auto schedule_data = iter.second;
                    schedule_data->_start_function.Call( schedule_data->_object_id, schedule_list->_duration_time );
                }
            }

            break;
        }
    }

    void KFScheduleModule::ExecuteScheduleFinish( std::shared_ptr<KFScheduleDataList> schedule_list, KFDate& now_date )
    {
        // 没有过结束时间
        if ( !KFDate::CheckSectionTimeData( &schedule_list->_finish_time_data, now_date ) )
        {
            return;
        }

        schedule_list->_status = KFScheduleEnum::Stop;

        // 功能函数回调
        for ( auto& iter : schedule_list->_schedule_data_list._objects )
        {
            auto schedule_data = iter.second;
            schedule_data->_finish_function.Call( schedule_data->_object_id, schedule_list->_duration_time );
        }
    }
}