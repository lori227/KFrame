#include "KFrame.h"
#include "KFSpdLog.h"
#include "KFLoggerConfig.hpp"

#if __KF_SYSTEM__ == __KF_LINUX__
    #include <execinfo.h>
#endif

namespace KFrame
{
    KFLogger* KFLogger::_kf_logger = nullptr;
    KFLogger::~KFLogger()
    {
        __DELETE_OBJECT__( _local_log );
        __DELETE_OBJECT__( _logger_config );
    }

    void KFLogger::Initialize( KFLogger* logger )
    {
        if ( logger == nullptr )
        {
            logger = __NEW_OBJECT__( KFLogger );
        }

        KFLogger::_kf_logger = logger;
    }

    KFLogger* KFLogger::Instance()
    {
        return KFLogger::_kf_logger;
    }

    bool KFLogger::InitLogger( const std::string& file )
    {
        try
        {
            _logger_config = __NEW_OBJECT__( KFLoggerConfig );
            _logger_config->LoadConfig( file );
        }
        catch ( const std::exception& )
        {
        }

        auto setting = _logger_config->FindSetting( _logger_config->_default_log_name );
        if ( setting == nullptr )
        {
            return false;
        }

        SetLogLevel( setting->_level );

        auto global = KFGlobal::Instance();
        _local_log = __NEW_OBJECT__( KFSpdLog, setting );
        _local_log->Initialize( global->_app_name, global->_app_type, global->_app_id->ToString() );

        // 注册函数
        RegisterLogFunction( this, &KFLogger::Log );
        return true;
    }

    void KFLogger::SetLogLevel( uint32 level )
    {
        _log_level = level;
    }

    void KFLogger::SetLogFunction( KFLogFunction& function )
    {
        if ( function != nullptr )
        {
            _log_function = function;
        }
        else
        {
            RegisterLogFunction( this, &KFLogger::Log );
        }
    }

    void KFLogger::Log( uint32 level, const std::string& content )
    {
        _local_log->Log( level, content );
    }

    KFSpdLog* KFLogger::NewLogger( uint64 id, const std::string& name, const std::string& file_name )
    {
        auto iter = _create_loggers.find( id );
        if ( iter != _create_loggers.end() )
        {
            return iter->second;
        }

        auto setting = _logger_config->FindSetting( name );
        if ( setting == nullptr )
        {
            return nullptr;
        }

        auto logger = __KF_NEW__( KFSpdLog, setting );

        logger->Initialize( file_name );
        _create_loggers[ id ] = logger;
        return logger;
    }

    void KFLogger::DeleteLogger( uint64 id )
    {
        auto iter = _create_loggers.find( id );
        if ( iter == _create_loggers.end() )
        {
            return;
        }

        __KF_DELETE__( KFSpdLog, iter->second );
        _create_loggers.erase( iter );
    }

    void KFLogger::LogStack( const std::string& error )
    {
#if __KF_SYSTEM__ == __KF_WIN__


#else
        Log( KFLogEnum::Error, error );
        Log( KFLogEnum::Error, "========================================" );

        static const int32 size = 1024;
        void* array[ size ];
        auto stack_num = backtrace( array, size );
        auto stack_trace = backtrace_symbols( array, stack_num );
        for ( auto i = 0; i < stack_num; ++i )
        {
            Log( KFLogEnum::Error, stack_trace[ i ] );
        }
        free( stack_trace );

        Log( KFLogEnum::Error, "========================================" );
#endif
    }
}