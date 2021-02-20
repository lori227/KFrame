#include "KFSpdLog.h"
#include "spdlog/spdlog.h"
#include "spdlog/async_logger.h"
#include "spdlog/details/thread_pool.h"
#include "spdlog/sinks/step_file_sink.h"
#include "spdlog/sinks/glog_file_sink.h"
#include "spdlog/sinks/date_and_hour_file_sink.h"
#include "KFLoggerConfig.hpp"

namespace KFrame
{
    KFSpdLog::KFSpdLog( std::shared_ptr<const KFLoggerSetting>& setting )
    {
        _setting = setting;
    }

    KFSpdLog::~KFSpdLog()
    {
        spdlog::drop( _normal_logger->name() );
    }

    void KFSpdLog::Initialize( const std::string& app_name, const std::string& app_type, const std::string& app_id )
    {
        auto filename = __FORMAT__( "{}-{}-{}.log", app_name, app_type, app_id );
        Initialize( filename );
    }

    void KFSpdLog::Initialize( const std::string& file_name )
    {
        auto log_name = __FORMAT__( "{}{}", _setting->_output_path, file_name );
        InitLogger( _normal_logger, log_name );
    }

    void KFSpdLog::InitLogger( std::shared_ptr<spdlog::logger>& logger, const std::string& log_name )
    {
        std::vector<spdlog::sink_ptr> sinks_vec;
        if ( _setting->_console )
        {
#if __KF_SYSTEM__ == __KF_WIN__
            auto colorsink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
            auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
            sinks_vec.push_back( color_sink );
        }

        switch ( _setting->_sink_type )
        {
        case KFSinkEnum::DateAndHour:
            sinks_vec.push_back( std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>( log_name ) );
            break;
        case KFSinkEnum::StepFile:
            sinks_vec.push_back( std::make_shared<spdlog::sinks::step_file_sink_mt>( log_name, _setting->_step_seconds, _setting->_max_log_size ) );
            break;
        case KFSinkEnum::GLogFile:
            sinks_vec.push_back( std::make_shared<spdlog::sinks::glog_file_sink_mt>( log_name, _setting->_split, _setting->_step_seconds, _setting->_max_log_size ) );
            break;
        default:
            break;
        }

        if ( _setting->_queue_count == 0u )
        {
            logger = std::make_shared<spdlog::logger>( log_name, std::begin( sinks_vec ), std::end( sinks_vec ) );
        }
        else
        {
            _thread_pool = std::make_shared<spdlog::details::thread_pool>( _setting->_queue_count, 1 );
            logger = std::make_shared<spdlog::async_logger>( log_name, std::begin( sinks_vec ), std::end( sinks_vec ), _thread_pool );
        }

        logger->set_pattern( _setting->_pattern );
        logger->set_level( spdlog::level::level_enum::trace );
        logger->flush_on( spdlog::level::level_enum::trace );
        spdlog::register_logger( logger );
    }

    void KFSpdLog::Log( uint32 log_level, const std::string& content )
    {
        _normal_logger->log( ( spdlog::level::level_enum )log_level, content );
    }
}
