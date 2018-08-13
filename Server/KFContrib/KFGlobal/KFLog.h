#ifndef __LOG_H__
#define __LOG_H__

#include "KFrame.h"
#include "spdlog/spdlog.h"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    //#define KF_LOCAL_LOG_TRACE(category, my_fmt, ...)		KF_LOCAL_LOG(spdlog::level::trace, category, my_fmt, ##__VA_ARGS__);
    //#define KF_LOCAL_LOG_DEBUG(category, my_fmt, ...)		KF_LOCAL_LOG(spdlog::level::debug, category, my_fmt, ##__VA_ARGS__);
    //#define KF_LOCAL_LOG_INFO(category, my_fmt, ...)		KF_LOCAL_LOG(spdlog::level::info, category, my_fmt, ##__VA_ARGS__);
    //#define KF_LOCAL_LOG_WARN(category, my_fmt, ...)		KF_LOCAL_LOG(spdlog::level::warn, category, my_fmt, ##__VA_ARGS__);
    //#define KF_LOCAL_LOG_ERROR(category, my_fmt, ...)		KF_LOCAL_LOG(spdlog::level::err, category, my_fmt, ##__VA_ARGS__);
    //#define KF_LOCAL_LOG_CRITICAL(category, my_fmt, ...)	KF_LOCAL_LOG(spdlog::level::critical, category, my_fmt, ##__VA_ARGS__);
    //#define KF_LOCAL_LOG(log_level, category, my_fmt, ...) KFGlobal::Instance()->_logger->Log(log_level, category, my_fmt, __FUNC_LINE__, ##__VA_ARGS__);
    //#define KF_LOCAL_LOG_WITHOUT_FUNCTION(log_level, category, my_fmt, ...) KFGlobal::Instance()->_logger->LogWithoutFunction(log_level, category, __FUNC_LINE__, my_fmt, ##__VA_ARGS__);

    class KFLog
    {
    public:
        using spdlogger = std::shared_ptr<spdlog::logger>;

        ~KFLog();

        // 初始化
        bool Initialize( const std::string& path, const std::string& appname, const std::string& apptype, uint32 appid );

        // 打印日志
        void Log( uint32 loglevel, uint32 category, const std::string& content );

    protected:
        const spdlogger& GetLogger( uint32 category );

        const spdlogger& CreateLogger( uint32 category );
        std::string GetLoggerName( uint32 category );

    private:
        std::string _local_log_path;
        std::map<std::string, spdlogger> _local_loggers;
    };

}

#endif
