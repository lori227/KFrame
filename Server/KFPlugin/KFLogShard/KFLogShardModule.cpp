#include "KFLogShardModule.h"
#include "spdlog/contrib/sinks/date_and_hour_file_sink.h"

namespace KFrame
{

#ifdef __USE_WORKER__
    #define __REGISTER_LOG_MESSAGE__ __REGISTER_WORKER_MESSAGE__
    #define __UNREGISTER_LOG_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
#else
    #define __REGISTER_LOG_MESSAGE__ __REGISTER_MESSAGE__
    #define __UNREGISTER_LOG_MESSAGE__ __UNREGISTER_MESSAGE__
#endif

    KFLogShardModule::~KFLogShardModule()
    {
        spdlog::drop_all();
    }

    void KFLogShardModule::BeforeRun()
    {
        __REGISTER_LOG_MESSAGE__( KFMsg::S2S_LOG_REQ, &KFLogShardModule::HandleLogReq );
    }

    void KFLogShardModule::BeforeShut()
    {
        __UNREGISTER_LOG_MESSAGE__( KFMsg::S2S_LOG_REQ );
    }

    //////////////////////////////////////////////////////////////////////////

    void KFLogShardModule::Log( const ELogCategory& category, const int log_level, const uint32 zone_id, const std::string& app_name, const std::string& app_type, const uint32 app_id, const std::string& log_info )
    {
        if ( log_level < spdlog::level::trace || log_level >= spdlog::level::off )
        {
            assert( 0 );
            return;
        }

        auto& logger = GetLogger( category, zone_id, app_name, app_type, app_id );
        if ( logger == nullptr )
        {
            return;
        }

        logger->log( ( spdlog::level::level_enum )log_level, log_info.c_str() );
    }

    void KFLogShardModule::CreateLogger( const std::string& zone_name, const std::string& app_name, const std::string& app_type, const uint32 app_id, const std::string& category )
    {
        std::vector<spdlog::sink_ptr> sinks_vec;
        std::string log_name;

#if __KF_SYSTEM__ == __KF_WIN__
        log_name = KF_FORMAT( "..\\binlog\\{}\\{}-{}-{}-{}.log", zone_name, app_name, app_type, app_id, category );
#else
        log_name = KF_FORMAT( "../binlog/{}/{}-{}-{}-{}.log", zone_name, app_name, app_type, app_id, category );
#endif

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

        std::string name = KF_FORMAT( "{}-{}-{}-{}-{}", zone_name, app_name, app_type, app_id, category );
        auto remote_logger = std::make_shared<spdlog::async_logger>( name, std::begin( sinks_vec ), std::end( sinks_vec ), 1024 );

#if defined(__KF_DEBUG__)
        remote_logger->set_level( spdlog::level::level_enum::trace );
        remote_logger->set_pattern( "%^[%Y%m%d %H:%M:%S.%e][%l]%v%$" );
        remote_logger->flush_on( spdlog::level::trace );
#else
        remote_logger->set_pattern( "[%Y%m%d %H:%M:%S.%e][%l]%v" );
        remote_logger->flush_on( spdlog::level::warn );
#endif

        spdlog::register_logger( remote_logger );

        _loggers.insert( std::make_pair( name, remote_logger ) );
    }

    const std::shared_ptr<spdlog::logger>& KFLogShardModule::GetLogger( const ELogCategory& category, const uint32 zone_id, const std::string& app_name, const std::string& app_type, const uint32 app_id )
    {
        static std::shared_ptr<spdlog::logger> NULLPTR_ = nullptr;

        if ( category <= ELogCategory::ELC_NONE || category >= ELogCategory::ELC_MAX )
        {
            assert( 0 );
            return NULLPTR_;
        }

        std::string zone_name = __TO_STRING__( zone_id );
        if ( zone_id == 0 )
        {
            zone_name = "cluster";
        }

        std::string logger_unique_name = KF_FORMAT( "{}-{}-{}-{}-{}", zone_name, app_name, app_type, app_id, LogCategoryName[category] );
        auto iter = _loggers.find( logger_unique_name );
        if ( iter == _loggers.end() )
        {
            CreateLogger( zone_name, app_name, app_type, app_id, LogCategoryName[category] );
        }

        iter = _loggers.find( logger_unique_name );
        return ( iter == _loggers.end() ) ? NULLPTR_ : iter->second;
    }

    //////////////////////////////////////////////////////////////////////////

    __KF_MESSAGE_FUNCTION__( KFLogShardModule::HandleLogReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLogReq );

        Log( ( ELogCategory )kfmsg.log_category(), kfmsg.log_level(), kfmsg.zone_id(), kfmsg.app_name(), kfmsg.app_type(), kfmsg.app_id(), kfmsg.log_info() );
    }
}