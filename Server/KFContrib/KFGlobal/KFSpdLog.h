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
        bool Initialize( const std::string& path, const std::string& appname, const std::string& apptype, const std::string& strappid );

        // 打印日志
        void Log( uint32 loglevel, const std::string& content );

    protected:

        // 创建log
        void CreateLogger( const std::string& path, const std::string& appname, const std::string& apptype, const std::string& strappid );

    private:
        // 日志对象
        spdlogger _logger;
    };

}

#endif
