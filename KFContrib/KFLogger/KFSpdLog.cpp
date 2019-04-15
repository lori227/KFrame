#include "KFSpdLog.h"
#include "KFMacros.h"
#include "spdlog/sinks/date_and_hour_file_sink.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
#endif // _WIN32

namespace KFrame
{
    KFSpdLog::KFSpdLog( bool console /* = true */, uint32 queuecount /* = 1024 */ )
    {
        _console = console;
        _queue_count = queuecount;
    }

    KFSpdLog::~KFSpdLog()
    {
        spdlog::drop( _log_name );
    }

    bool KFSpdLog::Initialize( const std::string& path, const std::string& appname, const std::string& apptype, const std::string& strappid )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        _log_name = __FORMAT__( "{}\\{}-{}-{}.log", path, appname, apptype, strappid );
#else
        _log_name = __FORMAT__( "{}/{}-{}-{}.log", path, appname, apptype, strappid );
#endif

        CreateLogger();
        return true;
    }

    void KFSpdLog::Log( uint32 loglevel, const std::string& content )
    {
        _logger->log( ( spdlog::level::level_enum )loglevel, content );
    }

    void KFSpdLog::CreateLogger()
    {
        std::vector<spdlog::sink_ptr> sinksvec;
        if ( _console )
        {
#if __KF_SYSTEM__ == __KF_WIN__
            auto colorsink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
            auto colorsink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
            sinksvec.push_back( colorsink );
        }

        sinksvec.push_back( std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>( _log_name ) );

        if ( _queue_count == 0 )
        {
            _logger = std::make_shared<spdlog::logger>( _log_name, std::begin( sinksvec ), std::end( sinksvec ) );
        }
        else
        {
            _thread_pool = std::make_shared<spdlog::details::thread_pool>( _queue_count, 1 );
            _logger = std::make_shared<spdlog::async_logger>( _log_name, std::begin( sinksvec ), std::end( sinksvec ), _thread_pool );
        }

#if defined(__KF_DEBUG__)
        _logger->set_pattern( "%^[%Y%m%d %H:%M:%S.%e][%l]%v%$" );
#else
        _logger->set_pattern( "[%Y%m%d %H:%M:%S.%e][%l]%v" );
#endif
        _logger->set_level( spdlog::level::level_enum::trace );
        _logger->flush_on( spdlog::level::trace );

        spdlog::register_logger( _logger );
    }
}
