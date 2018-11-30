#include "KFSpdLog.h"
#include "spdlog/contrib/sinks/date_and_hour_file_sink.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
#endif // _WIN32


namespace KFrame
{
    KFSpdLog::~KFSpdLog()
    {
        spdlog::drop_all();
    }

    bool KFSpdLog::Initialize( const std::string& path, const std::string& appname, const std::string& apptype, const std::string& strappid )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        _local_log_path = __FORMAT__( "{}\\{}-{}-{}", path, appname, apptype, strappid );
#else
        _local_log_path = __FORMAT__( "{}//{}-{}-{}", path, appname, apptype, strappid );
#endif
        return true;
    }

    void KFSpdLog::Log( uint32 loglevel, const std::string& content )
    {
        auto& logger = GetLogger();
        if ( logger == nullptr )
        {
            return;
        }

        logger->log( ( spdlog::level::level_enum )loglevel, content );
    }

    const KFSpdLog::spdlogger& KFSpdLog::GetLogger()
    {
        std::string name = GetLoggerName();
        auto iter = _local_loggers.find( name );
        if ( iter != _local_loggers.end() )
        {
            return iter->second;
        }

        return CreateLogger();
    }

    const KFSpdLog::spdlogger& KFSpdLog::CreateLogger()
    {
        std::vector<spdlog::sink_ptr> sinks_vec;
        std::string log_name = __FORMAT__( "{}.log", _local_log_path );

        auto date_and_hour_sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>( log_name );

#if defined(__KF_DEBUG__)
#if __KF_SYSTEM__ == __KF_WIN__
        auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
        auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
        sinks_vec.push_back( color_sink );
#endif

        sinks_vec.push_back( date_and_hour_sink );

        std::string name = GetLoggerName();
        auto local_logger = std::make_shared<spdlog::async_logger>( name, std::begin( sinks_vec ), std::end( sinks_vec ), 1024 );

#if defined(__KF_DEBUG__)
        local_logger->set_pattern( "%^[%Y%m%d %H:%M:%S.%e][%l]%v%$" );
#else
        local_logger->set_pattern( "[%Y%m%d %H:%M:%S.%e][%l]%v" );
#endif
        local_logger->set_level( spdlog::level::level_enum::trace );
        local_logger->flush_on( spdlog::level::trace );

        spdlog::register_logger( local_logger );
        auto iter = _local_loggers.insert( std::make_pair( name, local_logger ) ).first;
        return iter->second;
    }

    std::string KFSpdLog::GetLoggerName()
    {
        std::string name;
        std::string log_name = __FORMAT__( "{}", _local_log_path );
        size_t pos = log_name.find_last_of( spdlog::details::os::folder_sep );
        if ( pos == std::string::npos )
        {
            name = log_name;
        }
        else
        {
            name = log_name.substr( pos + 1, log_name.length() - pos - 1 );
        }

        return name;
    }
}
