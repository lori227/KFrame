#ifndef __SPD_LOG_H__
#define __SPD_LOG_H__

#include "KFInclude.h"
#include "spdlog/spdlog.h"
#include "spdlog/async_logger.h"
#include "spdlog/details/thread_pool.h"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    class KFSpdLog
    {
    public:
        using spdlogger = std::shared_ptr<spdlog::logger>;

        KFSpdLog( bool console = true, uint32 queuecount = 1024 );
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

        // 队列数量
        uint32 _queue_count;

        // 名字
        std::string _log_name;

        // 日志对象
        spdlogger _logger;

        // 线程池
        std::shared_ptr< spdlog::details::thread_pool > _thread_pool;
    };

}

#endif
