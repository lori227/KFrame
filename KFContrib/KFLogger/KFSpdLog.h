#ifndef __SPD_LOG_H__
#define __SPD_LOG_H__

#include "KFDefine.h"
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
        KFSpdLog( std::shared_ptr<const KFLoggerSetting>& setting );
        ~KFSpdLog();

        // 初始化
        void Initialize( const std::string& file_name );
        void Initialize( const std::string& app_name, const std::string& app_type, const std::string& app_id );

        // 打印日志
        void Log( uint32 log_level, const std::string& content );

    protected:
        // 初始化logger
        void InitLogger( std::shared_ptr< spdlog::logger >& logger, const std::string& log_name );

    private:
        // 日志配置
        std::shared_ptr<const KFLoggerSetting> _setting = nullptr;

        // 日志对象
        std::shared_ptr< spdlog::logger > _normal_logger;

        // 线程池
        std::shared_ptr< spdlog::details::thread_pool > _thread_pool;
    };

}

#endif
