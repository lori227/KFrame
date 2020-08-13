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
    KFSpdLog::KFSpdLog( const KFLoggerSetting* kfsetting )
    {
        _kf_setting = kfsetting;
    }

    KFSpdLog::~KFSpdLog()
    {
        spdlog::drop( _normal_logger->name() );
    }

    void KFSpdLog::Initialize( const std::string& appname, const std::string& apptype, const std::string& appid )
    {
        auto filename = __FORMAT__( "{}-{}-{}.log", appname, apptype, appid );
        Initialize( filename );
    }

    void KFSpdLog::Initialize( const std::string& filename )
    {
        auto logname = __FORMAT__( "{}{}", _kf_setting->_output_path, filename );
        InitLogger( _normal_logger, logname );
    }

    void KFSpdLog::InitLogger( std::shared_ptr<spdlog::logger>& logger, const std::string& logname )
    {
        std::vector<spdlog::sink_ptr> sinksvec;
        if ( _kf_setting->_console )
        {
#if __KF_SYSTEM__ == __KF_WIN__
            auto colorsink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
            auto colorsink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
            sinksvec.push_back( colorsink );
        }

        switch ( _kf_setting->_sink_type )
        {
        case KFSinkEnum::DateAndHour:
            sinksvec.push_back( std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>( logname ) );
            break;
        case KFSinkEnum::StepFile:
            sinksvec.push_back( std::make_shared<spdlog::sinks::step_file_sink_mt>( logname, _kf_setting->_step_seconds, _kf_setting->_max_log_size ) );
            break;
        case KFSinkEnum::GLogFile:
            sinksvec.push_back( std::make_shared<spdlog::sinks::glog_file_sink_mt>( logname, _kf_setting->_split, _kf_setting->_step_seconds, _kf_setting->_max_log_size ) );
            break;
        default:
            break;
        }

        if ( _kf_setting->_queue_count == 0u )
        {
            logger = std::make_shared<spdlog::logger>( logname, std::begin( sinksvec ), std::end( sinksvec ) );
        }
        else
        {
            _thread_pool = std::make_shared<spdlog::details::thread_pool>( _kf_setting->_queue_count, 1 );
            logger = std::make_shared<spdlog::async_logger>( logname, std::begin( sinksvec ), std::end( sinksvec ), _thread_pool );
        }

        logger->set_pattern( _kf_setting->_pattern );
        logger->set_level( spdlog::level::level_enum::trace );
        logger->flush_on( spdlog::level::level_enum::trace );
        spdlog::register_logger( logger );
    }

    void KFSpdLog::Log( uint32 loglevel, const std::string& content )
    {
        _normal_logger->log( ( spdlog::level::level_enum )loglevel, content );
    }
}
