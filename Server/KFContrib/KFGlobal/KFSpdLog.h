#ifndef __SPD_LOG_H__
#define __SPD_LOG_H__

#include "KFrame.h"
#include "spdlog/spdlog.h"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    class KFSpdLog
    {
    public:
        using spdlogger = std::shared_ptr<spdlog::logger>;

        ~KFSpdLog();

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
