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
        void Initialize( const std::string& filename );
        void Initialize( const std::string& appname, const std::string& apptype, const std::string& appid );

        // 打印日志
        void Log( uint32 loglevel, const std::string& content );

    protected:
        // 初始化logger
        void InitLogger( std::shared_ptr< spdlog::logger >& logger, const std::string& logname );

    private:
        // 日志配置
        const KFLoggerSetting* _kf_setting = nullptr;

        // 日志对象
        std::shared_ptr< spdlog::logger > _normal_logger;

        // 线程池
        std::shared_ptr< spdlog::details::thread_pool > _thread_pool;
    };

}

#endif
