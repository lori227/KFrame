#ifndef __LOG_MODULE_H__
#define __LOG_MODULE_H__

#include "KFInclude.h"
#include "KFMacros.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "KFPlugin/KFGlobal.h"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    class KFLogger
    {
    public:
        enum EConstDefine
        {
            //////////////////////////////////////////
            Trace = 1,
            Info = 2,
            Warn = 3,
            Error = 4,
            Debug = 5,
        };

        // 初始化
        static bool Initialize( const std::string& appname, const std::string& apptype, uint32 appid, std::string& file );

        // System日志
        template <class...P>
        static void LogSystem( uint32 type, const char* format, P&& ... params )
        {
            static const std::string _system_log_name = "System.log";

            SetConsoleTextColor( type );
            LogMessage( _system_log_name, type, format, std::forward< P >( params )... );
        }

        //// Load日志
        //template <class...P>
        //static void LogInit( uint32 type, const char* format, P&& ... params )
        //{
        //    static const std::string _init_log_name = "Init.log";

        //    SetConsoleTextColor( type );
        //    LogMessage( _init_log_name, type, format, std::forward< P >( params )... );
        //}

        // logic日志
        template <class...P>
        static void LogLogic( uint32 type, const char* format, P&& ... params )
        {
            static const std::string _logic_log_name = "Logic.log";
            LogMessage( _logic_log_name, type, format, std::forward< P >( params )... );
        }

        // 网络日志
        template <class...P>
        static void LogNet( uint32 type, const char* format, P&& ... params )
        {
            static const std::string _net_log_name = "Net.log";
            LogMessage( _net_log_name, type, format, std::forward< P >( params )... );
        }

        // SQL日志
        template <class...P>
        static void LogSql( uint32 type, const char* format, P&& ... params )
        {
            static const std::string _sql_log_name = "SQL.log";
            LogMessage( _sql_log_name, type, format, std::forward< P >( params )... );
        }

        // 内存日志
        template <class...P>
        static void LogMemory( uint32 type, const char* format, P&& ... params )
        {
            static const std::string _memory_log_name = "Memory.log";
            LogMessage( _memory_log_name, type, format, std::forward< P >( params )... );
        }

        // 脚本日志
        template <class...P>
        static void LogScript( uint32 type, const char* format, P&& ... params )
        {
            static const std::string _script_log_name = "Script.log";
            LogMessage( _script_log_name, type, format, std::forward< P >( params )... );
        }

        // 登录日志
        template <class...P>
        static void LogLogin( uint32 type, const char* format, P&& ... params )
        {
            static const std::string _login_log_name = "Login.log";
            LogMessage( _login_log_name, type, format, std::forward< P >( params )... );
        }

        // 玩家日志
        template <class...P>
        static void LogPlayer( uint32 type, const char* format, P&& ... params )
        {
            static const std::string _player_log_name = "Player.log";
            LogMessage( _player_log_name, type, format, std::forward< P >( params )... );
        }

    protected:
        // 打印日志
        static void LogMessage( const std::string& object, uint32 type, const char* format, ... );

        // 设置console颜色
        static void SetConsoleTextColor( uint32 type );

        // 格式化log文件名
        static std::string FormatLoggerFile( const std::string& apptype, uint32 appid, std::string& file );
        static std::string SaveLoggerFile( const std::string& appname, const std::string& apptype, uint32 appid, std::string& file );

    private:
        // 控制台句柄，用来设置字体颜色
        static void* _console_handle;
    };

#define KF_FORMAT(my_fmt, ...)				fmt::format(my_fmt, ##__VA_ARGS__);
#define KF_FORMAT_FUNCTION(my_fmt, ...)		fmt::format(std::string("[{}:{}]") + my_fmt, __FUNCTION_LINE__, ##__VA_ARGS__);

#define KF_LOG_TRACE(my_fmt, ...)			KFGlobal::Instance()->_logger->Log(spdlog::level::trace, __FUNCTION_LINE__, my_fmt, ##__VA_ARGS__);
#define KF_LOG_DEBUG(my_fmt, ...)			KFGlobal::Instance()->_logger->Log(spdlog::level::debug, __FUNCTION_LINE__, my_fmt, ##__VA_ARGS__);
#define KF_LOG_INFO(my_fmt, ...)			KFGlobal::Instance()->_logger->Log(spdlog::level::info, __FUNCTION_LINE__, my_fmt, ##__VA_ARGS__);
#define KF_LOG_WARN(my_fmt, ...)			KFGlobal::Instance()->_logger->Log(spdlog::level::warn, __FUNCTION_LINE__, my_fmt, ##__VA_ARGS__);
#define KF_LOG_ERROR(my_fmt, ...)			KFGlobal::Instance()->_logger->Log(spdlog::level::err, __FUNCTION_LINE__, my_fmt, ##__VA_ARGS__);
#define KF_LOG_CRITICAL(my_fmt, ...)		KFGlobal::Instance()->_logger->Log(spdlog::level::critical, __FUNCTION_LINE__, my_fmt, ##__VA_ARGS__);


    class KFLoggerEx
    {
    public:
        ~KFLoggerEx();

        bool Initialize( const std::string& path, const std::string& appname, const std::string& apptype, uint32 appid );

        template<typename... ARGS>
        void Log( spdlog::level::level_enum log_level, const char* function, int line, const char* my_fmt, ARGS&& ... args )
        {
            auto& logger = GetLogger();
            if ( logger == nullptr )
            {
                return;
            }
            else
            {
                std::string new_fmt = std::string( "[{}:{}]" ) + my_fmt;
                logger->log( log_level, new_fmt.c_str(), function, line, std::forward<ARGS>( args )... );
            }
        }

        const std::shared_ptr<spdlog::logger>& GetLogger();

    private:
        std::shared_ptr<spdlog::logger> _init_logger;
    };
}

#endif
