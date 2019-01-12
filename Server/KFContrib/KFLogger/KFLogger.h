#ifndef __KF_LOGGER_H__
#define __KF_LOGGER_H__

#include "spdlog/spdlog.h"

namespace KFrame
{
    // 日志函数
    typedef std::function< void( uint32, const std::string& ) > KFLogFunction;

    class KFSpdLog;
    class KFLogger
    {
    public:
        ~KFLogger();

        // 创建
        static void Initialize( KFLogger* kflogger );

        // 接口
        static KFLogger* Instance();

        // 初始化日志
        void InitLogger();

        // 设置日志登录
        void SetLogLevel( uint32 level );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterLogFunction( T* object, void ( T::*handle )( uint32, const std::string& ) )
        {
            KFLogFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            SetLogFunction( function );
        }

        template< class T >
        void UnRegisterLogFunction( T* object )
        {
            KFLogFunction function = nullptr;
            SetLogFunction( function );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 打印日志
        template<typename... P>
        void Log( uint32 level, const char* function, uint32 line, const std::string& myfmt, P&& ... args )
        {
            if ( _kf_logger == nullptr || level < _log_level )
            {
                return;
            }

            static const std::string _function_line_fmt = "[{}:{}] ";

            auto newfmt = _function_line_fmt + myfmt;
            auto content = __FORMAT__( newfmt, function, line, std::forward<P>( args )... );
            _log_function( level, content );
        }

    protected:
        KFLogger() = default;

        // 接口
        static KFLogger* _kf_logger;

        // 打印日志
        void Log( uint32 level, const std::string& content );

        // 设置远程日志函数
        void SetLogFunction( KFLogFunction& function );

    private:
        // log打印级别
        uint32 _log_level = 0u;

        // 本地log
        KFSpdLog* _local_log = nullptr;

        // 远程日志
        KFLogFunction _log_function = nullptr;

        // 远程日志
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
#define __LOG_DEBUG__( myfmt, ...) \
    KFLogger::Instance()->Log( spdlog::level::debug, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_DEBUG_FUNCTION__( function, line, myfmt, ...) \
    KFLogger::Instance()->Log( spdlog::level::debug, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_INFO__( myfmt, ...) \
    KFLogger::Instance()->Log( spdlog::level::info, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_INFO_FUNCTION__( function, line, myfmt, ...) \
    KFLogger::Instance()->Log( spdlog::level::info, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_WARN__( myfmt, ...) \
    KFLogger::Instance()->Log( spdlog::level::warn, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_WARN_FUNCTION__( function, line, myfmt, ...) \
    KFLogger::Instance()->Log( spdlog::level::warn, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_ERROR__( myfmt, ...) \
    KFLogger::Instance()->Log( spdlog::level::err, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_ERROR_FUNCTION__( function, line, myfmt, ...) \
    KFLogger::Instance()->Log( spdlog::level::err, function, line, myfmt, ##__VA_ARGS__ )
}

#endif