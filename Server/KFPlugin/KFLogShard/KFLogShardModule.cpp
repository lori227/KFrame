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
        __REGISTER_LOG_MESSAGE__( KFMsg::S2S_LOG_REQ, &KFLogShardModule::HandleRemoteLogReq );
    }

    void KFLogShardModule::BeforeShut()
    {
        __UNREGISTER_LOG_MESSAGE__( KFMsg::S2S_LOG_REQ );
    }

    //////////////////////////////////////////////////////////////////////////

    void KFLogShardModule::Log( uint32 level, uint32 zoneid, const std::string& appname, const std::string& apptype, const std::string& strappid, const std::string& loginfo )
    {
        if ( level < spdlog::level::trace || level >= spdlog::level::off )
        {
            assert( 0 );
            return;
        }

        auto& logger = GetLogger( appname, apptype, strappid );
        if ( logger == nullptr )
        {
            return;
        }

        logger->log( ( spdlog::level::level_enum )level, loginfo.c_str() );
    }

    void KFLogShardModule::CreateLogger( const std::string& loggername )
    {
        std::vector<spdlog::sink_ptr> sinks_vec;

#if __KF_SYSTEM__ == __KF_WIN__
        std::string logpath = __FORMAT__( "..\\binlog\\{}", loggername );
#else
        std::string logpath = __FORMAT__( "../binlog/{}", loggername );
#endif

        auto date_and_hour_sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>( logpath );
#if __KF_SYSTEM__ == __KF_WIN__
        auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
        auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif

        sinks_vec.push_back( color_sink );
        sinks_vec.push_back( date_and_hour_sink );

        auto remote_logger = std::make_shared<spdlog::async_logger>( loggername, std::begin( sinks_vec ), std::end( sinks_vec ), 1024 );

#if defined(__KF_DEBUG__)
        remote_logger->set_pattern( "%^[%Y%m%d %H:%M:%S.%e][%l]%v%$" );
#else
        remote_logger->set_pattern( "[%Y%m%d %H:%M:%S.%e][%l]%v" );
#endif
        remote_logger->set_level( spdlog::level::level_enum::trace );
        remote_logger->flush_on( spdlog::level::trace );

        spdlog::register_logger( remote_logger );

        _loggers.insert( std::make_pair( loggername, remote_logger ) );
    }

    const std::shared_ptr<spdlog::logger>& KFLogShardModule::GetLogger( const std::string& appname, const std::string& apptype, const std::string& strappid )
    {
        static std::shared_ptr<spdlog::logger> NULLPTR_ = nullptr;

        std::string loggername = __FORMAT__( "{}-{}-{}.log", appname, apptype, strappid );
        auto iter = _loggers.find( loggername );
        if ( iter == _loggers.end() )
        {
            CreateLogger( loggername );
        }

        iter = _loggers.find( loggername );
        return ( iter == _loggers.end() ) ? NULLPTR_ : iter->second;
    }

    //////////////////////////////////////////////////////////////////////////

    __KF_MESSAGE_FUNCTION__( KFLogShardModule::HandleRemoteLogReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLogReq );

        Log( kfmsg.log_level(), kfmsg.zone_id(), kfmsg.app_name(), kfmsg.app_type(), kfmsg.app_id(), kfmsg.log_info() );
    }
}