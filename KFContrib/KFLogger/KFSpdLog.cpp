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
        spdlog::drop( _log_name );
    }

    void KFSpdLog::Initialize( const std::string& appname, const std::string& apptype, const std::string& strappid )
    {
        if ( _kf_setting->_file_name.empty() )
        {
            _log_name = __FORMAT__( "{}{}-{}-{}.log", _kf_setting->_output_path, appname, apptype, strappid );
        }
        else
        {
            _log_name = __FORMAT__( "{}{}{}{}-{}-{}.log",
                                    _kf_setting->_output_path, _kf_setting->_file_name, _kf_setting->_split,
                                    appname, apptype, strappid );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
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
            sinksvec.push_back( std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>( _log_name ) );
            break;
        case KFSinkEnum::StepFile:
            sinksvec.push_back( std::make_shared<spdlog::sinks::step_file_sink_mt>( _log_name, _kf_setting->_step_seconds, _kf_setting->_max_log_size ) );
            break;
        case KFSinkEnum::GLogFile:
            sinksvec.push_back( std::make_shared<spdlog::sinks::glog_file_sink_mt>( _log_name, _kf_setting->_split, _kf_setting->_step_seconds, _kf_setting->_max_log_size ) );
            break;
        default:
            break;
        }

        if ( _kf_setting->_queue_count == 0u )
        {
            _logger = std::make_shared<spdlog::logger>( _log_name, std::begin( sinksvec ), std::end( sinksvec ) );
        }
        else
        {
            _thread_pool = std::make_shared<spdlog::details::thread_pool>( _kf_setting->_queue_count, 1 );
            _logger = std::make_shared<spdlog::async_logger>( _log_name, std::begin( sinksvec ), std::end( sinksvec ), _thread_pool );
        }

        _logger->set_pattern( _kf_setting->_pattern );
        _logger->set_level( spdlog::level::level_enum::trace );
        _logger->flush_on( spdlog::level::level_enum::trace );
        spdlog::register_logger( _logger );
    }

    void KFSpdLog::Log( uint32 loglevel, const std::string& content )
    {
        _logger->log( ( spdlog::level::level_enum )loglevel, content );
    }
}
