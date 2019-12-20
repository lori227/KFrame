#ifndef __SPD_LOG_H__
#define __SPD_LOG_H__

#include "KFInclude.h"
#include "KFMacros.h"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
namespace spdlog
{
    class logger;
    namespace details
    {
        class thread_pool;
    }
}

namespace KFrame
{
    class KFLoggerSetting;
    class KFSpdLog
    {
    public:
        KFSpdLog( const KFLoggerSetting* kfsetting );
        ~KFSpdLog();

        // 初始化
        void Initialize( const std::string& appname, const std::string& apptype, const std::string& strappid );

        // 打印日志
        void Log( uint32 loglevel, const std::string& content );

    protected:

        // 创建log
        void CreateLogger();

    private:
        // 日志配置
        const KFLoggerSetting* _kf_setting = nullptr;

        // 名字
        std::string _log_name;

        // 日志对象
        std::shared_ptr< spdlog::logger > _logger;

        // 线程池
        std::shared_ptr< spdlog::details::thread_pool > _thread_pool;
    };

}

#endif
