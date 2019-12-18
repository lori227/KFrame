#include "KFrame.h"
#include "KFSpdLog.h"
#include "KFLoggerConfig.hpp"

namespace KFrame
{
    KFLogger* KFLogger::_kf_logger = nullptr;
    KFLogger::~KFLogger()
    {
        __DELETE_OBJECT__( _local_log );
        __DELETE_OBJECT__( _logger_config );
    }

    void KFLogger::Initialize( KFLogger* kflogger )
    {
        if ( kflogger == nullptr )
        {
            kflogger = new KFLogger();
        }

        KFLogger::_kf_logger = kflogger;
    }

    KFLogger* KFLogger::Instance()
    {
        return KFLogger::_kf_logger;
    }

    bool KFLogger::InitLogger( const std::string& name )
    {
        try
        {
            _logger_config = __NEW_OBJECT__( KFLoggerConfig );
            _logger_config->LoadConfig( "./setting/logger.xml" );
        }
        catch ( const std::exception& )
        {
        }

        auto kfsetting = _logger_config->FindSetting( name );
        if ( kfsetting == nullptr )
        {
            kfsetting = _logger_config->FindSetting( "normal" );
            if ( kfsetting == nullptr )
            {
                return false;
            }
        }

        SetLogLevel( kfsetting->_level );

        auto kfglobal = KFGlobal::Instance();
        _local_log = __NEW_OBJECT__( KFSpdLog, kfsetting );
        _local_log->Initialize( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );

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

    KFSpdLog* KFLogger::NewLogger( uint64 id, const std::string& name, const std::string& appname, const std::string& apptype, const std::string& appid )
    {
        auto iter = _create_loggers.find( id );
        if ( iter != _create_loggers.end() )
        {
            return iter->second;
        }

        auto kfsetting = _logger_config->FindSetting( name );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        auto kflogger = __KF_NEW__( KFSpdLog, kfsetting );
        kflogger->Initialize( appname, apptype, appid );
        _create_loggers[ id ] = kflogger;
        return kflogger;
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
}