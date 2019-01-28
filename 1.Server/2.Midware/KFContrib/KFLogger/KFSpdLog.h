#ifndef __SPD_LOG_H__
#define __SPD_LOG_H__

#include "KFInclude.h"
#include "spdlog/spdlog.h"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    class KFSpdLog
    {
    public:
        using spdlogger = std::shared_ptr<spdlog::logger>;

        KFSpdLog( bool console = true );
        ~KFSpdLog();

        // 初始化
        bool Initialize( const std::string& path, const std::string& appname, const std::string& apptype, const std::string& strappid );

        // 打印日志
        void Log( uint32 loglevel, const std::string& content );

    protected:

        // 创建log
        void CreateLogger();

    private:
        // 是否在Console上显示
        bool _console;
        // 名字
        std::string _log_name;

        // 日志对象
        spdlogger _logger;
    };

}

#endif
