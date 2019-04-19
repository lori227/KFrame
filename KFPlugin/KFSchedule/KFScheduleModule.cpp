#include "KFScheduleModule.hpp"

namespace KFrame
{
    KFScheduleModule::KFScheduleModule()
    {
        // 25秒检查一次, 太频繁浪费性能
        _update_timer.StartLoop( 25000 );
    }

    KFScheduleModule::~KFScheduleModule()
    {
        for ( auto iter : _kf_schedule_data )
        {
            for ( auto miter : iter.second )
            {
                __KF_DELETE__( KFScheduleData, miter.second );
            }
        }

        _kf_schedule_data.clear();
    }

    void KFScheduleModule::Run()
    {
        RunScheduleRegister();
        RunScheduleRemove();
        RunScheduleUpdate();
    }

    KFScheduleSetting* KFScheduleModule::CreateScheduleSetting()
    {
        return __KF_NEW__( KFScheduleTime );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFScheduleModule::AddSchedule( const std::string& module, KFScheduleSetting* kfsetting, KFScheduleFunction& function )
    {
        auto kftimerdata = __KF_NEW__( KFScheduleData );
        kftimerdata->_module = module;
        kftimerdata->_schedule_time = static_cast< KFScheduleTime* >( kfsetting );
        kftimerdata->_function = function;
        _kf_schedule_register.push_back( kftimerdata );
    }

    void KFScheduleModule::RemoveSchedule( const std::string& module )
    {
        RemoveSchedule( module, _invalid_int );
    }

    void KFScheduleModule::RemoveSchedule( const std::string& module, uint64 objectid )
    {
        auto remove = std::make_tuple( module, objectid );
        _kf_schedule_remove.push_back( remove );
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
            std::string module = "";
            uint64 objectid = 0u;
            std::tie( module, objectid ) = data;

            if ( objectid != _invalid_int )
            {
                RemoveScheduleData( module, objectid );
            }
            else
            {
                auto miter = _kf_schedule_data.find( module );
                if ( miter != _kf_schedule_data.end() )
                {
                    for ( auto& diter : miter->second )
                    {
                        RemoveScheduleData( module, diter.first );
                    }
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
            RemoveScheduleData( kfdata->_module, kfdata->_schedule_time->_object_id );
            AddSchedule( kfdata );
        }

        _kf_schedule_register.clear();
    }

    void KFScheduleModule::RunScheduleUpdate()
    {
        if ( !_update_timer.DoneTimer() )
        {
            return;
        }

        auto realtime = KFGlobal::Instance()->_real_time;
        KFDate kfdate( realtime );

        auto year = kfdate.GetYear();
        auto month = kfdate.GetMonth();
        auto day = kfdate.GetDay();
        auto hour = kfdate.GetHour();
        auto minute = kfdate.GetMinute();
        auto dayofweek = kfdate.GetDayOfWeek();

        for ( auto iter : _kf_schedule_data )
        {
            auto templist = iter.second;
            for ( auto miter : templist )
            {
                auto kfdata = miter.second;
                auto ok = kfdata->Execute( year, month, day, dayofweek, hour, minute );
                if ( ok )
                {
                    RemoveSchedule( iter.first, miter.first );
                }
            }
        }
    }

    void KFScheduleModule::AddSchedule( KFScheduleData* kfdata )
    {
        auto iter = _kf_schedule_data.find( kfdata->_module );
        if ( iter == _kf_schedule_data.end() )
        {
            std::map< uint64, KFScheduleData* > temp;
            iter = _kf_schedule_data.insert( std::make_pair( kfdata->_module, temp ) ).first;
        }

        iter->second.insert( std::make_pair( kfdata->_schedule_time->_object_id, kfdata ) );
    }

    void KFScheduleModule::RemoveScheduleData( const std::string& module, uint64 objectid )
    {
        auto iter = _kf_schedule_data.find( module );
        if ( iter == _kf_schedule_data.end() )
        {
            return;
        }

        auto miter = iter->second.find( objectid );
        if ( miter == iter->second.end() )
        {
            return;
        }

        __KF_DELETE__( KFScheduleData, miter->second );
        iter->second.erase( miter );
    }
}