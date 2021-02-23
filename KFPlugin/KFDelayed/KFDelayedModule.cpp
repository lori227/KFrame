#include "KFDelayedModule.hpp"

namespace KFrame
{
    KFDelayedModule::~KFDelayedModule()
    {
        _delayed_data_list.Clear();
        _delayed_remove_list.clear();
        _delayed_register_list.clear();
    }

    void KFDelayedModule::BeforeRun()
    {
        CalcNextCheckRunTime();
    }

    void KFDelayedModule::Run()
    {
        RunDelayedRemove();
        RunDelayedRegister();
        RunDelayedUpdate();
    }

    void KFDelayedModule::CalcNextCheckRunTime()
    {
        auto minute = KFGlobal::Instance()->_real_time / KFTimeEnum::OneMinuteSecond;
        _next_check_run_time = ( minute + 1u ) * KFTimeEnum::OneMinuteSecond;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDelayedModule::AddDelayedFunction( uint64 time, uint64 object_id, const int8* data, uint32 size,
            KFModule* module, KFDelayedFunction& function )
    {
        auto delayed_data = __MAKE_SHARED__( KFDelayedData, data, size );
        delayed_data->_loop_type = KFDelayedEnum::Once;
        delayed_data->_next_execute_time = time;
        delayed_data->_object_id = object_id;
        delayed_data->_function.SetFunction( module, function );
        _delayed_register_list.push_back( delayed_data );
    }

    void KFDelayedModule::AddDelayedFunction( KFTimeData* time_data, uint64 object_id, const int8* data, uint32 size,
            KFModule* module, KFDelayedFunction& function )
    {
        auto delayed_data = __MAKE_SHARED__( KFDelayedData, data, size );
        delayed_data->_loop_type = KFDelayedEnum::Loop;
        delayed_data->_time_data = *time_data;
        delayed_data->_next_execute_time = KFDate::CalcTimeData( time_data, KFGlobal::Instance()->_real_time, 1 );
        delayed_data->_object_id = object_id;
        delayed_data->_function.SetFunction( module, function );
        _delayed_register_list.push_back( delayed_data );
    }

    void KFDelayedModule::RemoveDelayedFunction( KFModule* module )
    {
        RemoveDelayedFunction( _invalid_int, module );
    }

    void KFDelayedModule::RemoveDelayedFunction( uint64 object_id, KFModule* module )
    {
        _delayed_remove_list.emplace_back( std::make_tuple( object_id, module ) );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDelayedModule::RunDelayedRemove()
    {
        // 删除无效和执行过的任务
        if ( _delayed_remove_list.empty() )
        {
            return;
        }

        for ( auto& tuple_data : _delayed_remove_list )
        {
            auto object_id = std::get<0>( tuple_data );
            auto module = std::get<1>( tuple_data );
            RemoveDelayedData( object_id, module );
        }

        _delayed_remove_list.clear();
    }

    void KFDelayedModule::RemoveDelayedData( uint64 object_id, KFModule* module )
    {
        auto module_delayed_list = _delayed_data_list.Find( module );
        if ( module_delayed_list == nullptr )
        {
            return;
        }

        if ( object_id != _invalid_int )
        {
            module_delayed_list->Remove( object_id );
        }
        else
        {
            module_delayed_list->Clear();
        }

        if ( module_delayed_list->IsEmpty() )
        {
            _delayed_data_list.Remove( module );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDelayedModule::RunDelayedRegister()
    {
        if ( _delayed_register_list.empty() )
        {
            return;
        }

        for ( auto delayed_data : _delayed_register_list )
        {
            RemoveDelayedData( delayed_data->_object_id, delayed_data->_function.GetModule() );
            AddDelayedData( delayed_data );
        }

        _delayed_register_list.clear();
    }

    void KFDelayedModule::AddDelayedData( std::shared_ptr<KFDelayedData> delayed_data )
    {
        auto module_delay_list = _delayed_data_list.Create( delayed_data->_function.GetModule() );
        module_delay_list->Insert( delayed_data->_object_id, delayed_data );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDelayedModule::RunDelayedUpdate()
    {
        if ( _next_check_run_time > KFGlobal::Instance()->_real_time )
        {
            return;
        }
        CalcNextCheckRunTime();

        for ( auto& iter : _delayed_data_list._objects )
        {
            for ( auto& object_iter : iter.second->_objects )
            {
                auto delayed_data = object_iter.second;
                if ( delayed_data->_next_execute_time < KFGlobal::Instance()->_real_time )
                {
                    continue;
                }

                // 时间到了, 执行回调
                delayed_data->_function.Call( delayed_data->_object_id, delayed_data->_data, delayed_data->_size );
                if ( delayed_data->_loop_type == KFDelayedEnum::Once )
                {
                    RemoveDelayedFunction(  delayed_data->_object_id, delayed_data->_function.GetModule() );
                }
                else
                {
                    // 计算下一次时间
                    delayed_data->_next_execute_time = KFDate::CalcTimeData( &delayed_data->_time_data, KFGlobal::Instance()->_real_time, 1 );
                    if ( delayed_data->_next_execute_time <= KFGlobal::Instance()->_real_time )
                    {
                        RemoveDelayedFunction( delayed_data->_object_id, delayed_data->_function.GetModule() );
                    }
                }
            }
        }
    }


}