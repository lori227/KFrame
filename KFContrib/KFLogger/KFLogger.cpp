#include "KFrame.h"
#include "KFSpdLog.h"

namespace KFrame
{
    KFLogger* KFLogger::_kf_logger = nullptr;

    KFLogger::~KFLogger()
    {
        __DELETE_OBJECT__( _local_log );
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

    void KFLogger::InitLogger( uint32 level, bool console /* = true */ )
    {
        _local_log = new KFSpdLog( console );
        SetLogLevel( level );

        auto kfglobal = KFGlobal::Instance();
        std::string outpath = __FORMAT__( ".{}_output", spdlog::details::os::folder_sep );
        _local_log->Initialize( outpath, kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );

        // 注册函数
        RegisterLogFunction( this, &KFLogger::Log );
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
}