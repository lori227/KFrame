#include "KFDelayedModule.hpp"

namespace KFrame
{
    KFDelayedModule::~KFDelayedModule()
    {
        for ( auto iter : _kf_delayed_data )
        {
            for ( auto miter : iter.second )
            {
                __KF_DELETE__( KFDelayedData, miter.second );
            }
        }
        _kf_delayed_data.clear();

        for ( auto data : _kf_delayed_register )
        {
            __KF_DELETE__( KFDelayedData, data );
        }
        _kf_delayed_register.clear();
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
    void KFDelayedModule::AddDelayedFunction( uint64 time, uint64 objectid, const int8* data, uint32 size,
            const std::string& module, KFDelayedFunction& function )
    {
        auto delayeddata = __KF_NEW__( KFDelayedData, data, size );
        delayeddata->_loop_type = KFDelayedEnum::Once;
        delayeddata->_module = module;
        delayeddata->_next_execute_time = time;
        delayeddata->_object_id = objectid;
        delayeddata->_function = function;
        _kf_delayed_register.push_back( delayeddata );
    }

    void KFDelayedModule::AddDelayedFunction( KFTimeData* timedata, uint64 objectid, const int8* data, uint32 size,
            const std::string& module, KFDelayedFunction& function )
    {
        auto delayeddata = __KF_NEW__( KFDelayedData, data, size );
        delayeddata->_loop_type = KFDelayedEnum::Loop;
        delayeddata->_module = module;
        delayeddata->_time_data = *timedata;
        delayeddata->_next_execute_time = KFDate::CalcTimeData( timedata, KFGlobal::Instance()->_real_time, 1 );
        delayeddata->_object_id = objectid;
        delayeddata->_function = function;
        _kf_delayed_register.push_back( delayeddata );
    }

    void KFDelayedModule::RemoveDelayedFunction( const std::string& module )
    {
        RemoveDelayedFunction( module, _invalid_int );
    }

    void KFDelayedModule::RemoveDelayedFunction( const std::string& module, uint64 objectid )
    {
        _kf_delayed_remove.emplace_back( std::make_tuple( module, objectid ) );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDelayedModule::RunDelayedRemove()
    {
        // 删除无效和执行过的任务
        if ( _kf_delayed_remove.empty() )
        {
            return;
        }

        for ( auto& data : _kf_delayed_remove )
        {
            std::string module = "";
            uint64 objectid = 0u;
            std::tie( module, objectid ) = data;
            RemoveDelayedData( module, objectid );
        }

        _kf_delayed_remove.clear();
    }

    void KFDelayedModule::RemoveDelayedData( const std::string& module, uint64 objectid )
    {
        auto iter = _kf_delayed_data.find( module );
        if ( iter == _kf_delayed_data.end() )
        {
            return;
        }

        if ( objectid != _invalid_int )
        {
            auto miter = iter->second.find( objectid );
            if ( miter != iter->second.end() )
            {
                __KF_DELETE__( KFDelayedData, miter->second );
                iter->second.erase( miter );
            }
        }
        else
        {
            for ( auto& miter : iter->second )
            {
                __KF_DELETE__( KFDelayedData, miter.second );
            }
            iter->second.clear();
        }

        if ( iter->second.empty() )
        {
            _kf_delayed_data.erase( iter );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDelayedModule::RunDelayedRegister()
    {
        if ( _kf_delayed_register.empty() )
        {
            return;
        }

        for ( auto kfdata : _kf_delayed_register )
        {
            RemoveDelayedData( kfdata->_module, kfdata->_object_id );
            AddDelayedData( kfdata );
        }

        _kf_delayed_register.clear();
    }

    void KFDelayedModule::AddDelayedData( KFDelayedData* kfdata )
    {
        auto iter = _kf_delayed_data.find( kfdata->_module );
        if ( iter == _kf_delayed_data.end() )
        {
            std::unordered_map< uint64, KFDelayedData* > temp;
            iter = _kf_delayed_data.emplace( std::make_pair( kfdata->_module, temp ) ).first;
        }

        iter->second.emplace( std::make_pair( kfdata->_object_id, kfdata ) );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDelayedModule::RunDelayedUpdate()
    {
        if ( _next_check_run_time > KFGlobal::Instance()->_real_time )
        {
            return;
        }
        CalcNextCheckRunTime();

        for ( auto& iter : _kf_delayed_data )
        {
            for ( auto& miter : iter.second )
            {
                auto delayeddata = miter.second;
                if ( delayeddata->_next_execute_time < KFGlobal::Instance()->_real_time )
                {
                    continue;
                }

                // 时间到了, 执行回调
                delayeddata->_function( delayeddata->_object_id, delayeddata->_data, delayeddata->_size );
                if ( delayeddata->_loop_type == KFDelayedEnum::Once )
                {
                    RemoveDelayedFunction( delayeddata->_module, delayeddata->_object_id );
                }
                else
                {
                    // 计算下一次时间
                    delayeddata->_next_execute_time = KFDate::CalcTimeData( &delayeddata->_time_data, KFGlobal::Instance()->_real_time, 1 );
                    if ( delayeddata->_next_execute_time <= KFGlobal::Instance()->_real_time )
                    {
                        RemoveDelayedFunction( delayeddata->_module, delayeddata->_object_id );
                    }
                }
            }
        }
    }


}