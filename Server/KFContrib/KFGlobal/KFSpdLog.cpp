#include "KFSpdLog.h"
#include "spdlog/contrib/sinks/date_and_hour_file_sink.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
#endif // _WIN32


namespace KFrame
{
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
        if ( _logger != nullptr )
        {
            _logger->log( ( spdlog::level::level_enum )loglevel, content );
        }
    }

    void KFSpdLog::CreateLogger()
    {
        std::vector<spdlog::sink_ptr> sinks_vec;

#if __KF_SYSTEM__ == __KF_WIN__
        auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
        auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
        sinks_vec.push_back( color_sink );

        auto date_and_hour_sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>( _log_name );

        sinks_vec.push_back( date_and_hour_sink );
        _logger = std::make_shared<spdlog::async_logger>( _log_name, std::begin( sinks_vec ), std::end( sinks_vec ), 1024 );

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
