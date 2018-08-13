#include "KFLog.h"
#include "spdlog/contrib/sinks/date_and_hour_file_sink.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
#endif // _WIN32


namespace KFrame
{
    KFLog::~KFLog()
    {
        spdlog::drop_all();
    }

    bool KFLog::Initialize( const std::string& path, const std::string& appname, const std::string& apptype, uint32 appid )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        _local_log_path = __FORMAT__( "{}\\{}-{}-{}", path, appname, apptype, appid );
#else
        _local_log_path = __FORMAT__( "{}//{}-{}-{}", path, appname, apptype, appid );
#endif
        return true;
    }

    void KFLog::Log( uint32 loglevel, uint32 category, const std::string& content )
    {
        auto& logger = GetLogger( category );
        if ( logger == nullptr )
        {
            return;
        }

        logger->log( ( spdlog::level::level_enum )loglevel, content );
    }

    const KFLog::spdlogger& KFLog::GetLogger( uint32 category )
    {
        std::string name = GetLoggerName( category );
        auto iter = _local_loggers.find( name );
        if ( iter != _local_loggers.end() )
        {
            return iter->second;
        }

        return CreateLogger( category );
    }

    const KFLog::spdlogger& KFLog::CreateLogger( uint32 category )
    {
        std::vector<spdlog::sink_ptr> sinks_vec;
        std::string log_name = __FORMAT__( "{}-{}.log", _local_log_path, _log_category_name[category] );

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

        std::string name = GetLoggerName( category );
        auto local_logger = std::make_shared<spdlog::async_logger>( name, std::begin( sinks_vec ), std::end( sinks_vec ), 1024 );

#if defined(__KF_DEBUG__)
        local_logger->set_level( spdlog::level::level_enum::trace );
        local_logger->set_pattern( "%^[%Y%m%d %H:%M:%S.%e][%l]%v%$" );
        local_logger->flush_on( spdlog::level::trace );
#else
        local_logger->set_pattern( "[%Y%m%d %H:%M:%S.%e][%l]%v" );
        local_logger->flush_on( spdlog::level::warn );
#endif

        spdlog::register_logger( local_logger );
        auto iter = _local_loggers.insert( std::make_pair( name, local_logger ) ).first;
        return iter->second;
    }

    std::string KFLog::GetLoggerName( uint32 category )
    {
        std::string log_name = __FORMAT__( "{}-{}", _local_log_path, _log_category_name[category] );
        std::string name;
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
