#include "KFLogger.h"
#include "KFSystem.h"
#include "log4cxx/logger.h"
#include "KFUtility/KFUtility.h"
#include "log4cxx/propertyconfigurator.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
#endif // _WIN32


namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    bool KFLogger::Initialize( const std::string& appname, const std::string& apptype, uint32 appid, std::string& file )
    {
        auto logfile = SaveLoggerFile( appname, apptype, appid, file );

        try
        {
            log4cxx::PropertyConfigurator::configure( logfile.c_str() );
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    std::string KFLogger::FormatLoggerFile( const std::string& apptype, uint32 appid, std::string& file )
    {
        auto temp = file;

        char filename[ 128 ] = "";
        sprintf( filename, "%s%u", apptype.c_str(), appid );
        KFUtility::ReplaceString( temp, "app", filename );

        return temp;
    }

    std::string KFLogger::SaveLoggerFile( const std::string& appname, const std::string& apptype, uint32 appid, std::string& file )
    {
        std::ifstream infile( file );
        if ( !infile )
        {
            return _invalid_str;
        }

        char filename[ 128 ] = "";
        sprintf( filename, "%s%u", apptype.c_str(), appid );

        std::ostringstream os;
        infile >> os.rdbuf();

        std::string filedata = os.str();
        KFUtility::ReplaceString( filedata, "$app", filename );
        infile.close();

        //////////////////////////////////////////////////////////////////////////////
        // 格式化文件名
        static std::string logfile = "";
        logfile = FormatLoggerFile( apptype, appid, file );

        std::ofstream offile( logfile );
        if ( !offile )
        {
            return _invalid_str;
        }

        offile.write( filedata.c_str(), filedata.size() );
        offile.close();

        return logfile;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFLogger::LogMessage( const std::string& object, uint32 type, const char* format, ... )
    {
        log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger( object.c_str() );
        if ( logger == nullptr )
        {
            return;
        }

        char buff[ 2048 ] = "";

        va_list args;
        va_start( args, format );
        vsprintf( buff, format, args );
        va_end( args );

        switch ( type )
        {
        case KFLogger::Trace:
            LOG4CXX_TRACE( logger, buff );
            break;
        case KFLogger::Info:
            LOG4CXX_INFO( logger, buff );
            break;
        case KFLogger::Warn:
            LOG4CXX_WARN( logger, buff );
            break;
        case KFLogger::Error:
            LOG4CXX_ERROR( logger, buff );
            break;
        case KFLogger::Debug:
            LOG4CXX_DEBUG( logger, buff );
            break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    // 控制台句柄，用来设置字体颜色
    void* KFLogger::_console_handle = nullptr;
    void KFLogger::SetConsoleTextColor( uint32 type )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        if ( _console_handle == nullptr )
        {
            _console_handle = GetStdHandle( STD_OUTPUT_HANDLE );
        }

        switch ( type )
        {
        case KFLogger::Trace:
            SetConsoleTextAttribute( _console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
            break;
        case KFLogger::Info:
            SetConsoleTextAttribute( _console_handle, FOREGROUND_GREEN );
            break;
        case KFLogger::Warn:
            SetConsoleTextAttribute( _console_handle, FOREGROUND_RED | FOREGROUND_GREEN );
            break;
        case KFLogger::Error:
            SetConsoleTextAttribute( _console_handle, FOREGROUND_RED );
            break;
        case KFLogger::Debug:
            SetConsoleTextAttribute( _console_handle, FOREGROUND_GREEN );
            break;
        }
#endif // _WIN32
    }
}
