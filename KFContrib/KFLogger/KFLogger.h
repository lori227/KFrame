#ifndef __KF_LOGGER_H__
#define __KF_LOGGER_H__

#include "KFMap.h"

namespace KFrame
{
    // 日志函数
    class KFSpdLog;
    class KFLoggerConfig;
    typedef std::function< void( uint32, const std::string& ) > KFLogFunction;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFLogger
    {
    public:
        ~KFLogger();

        // 创建
        static void Initialize( KFLogger* logger );

        // 接口
        static KFLogger* Instance();

        // 初始化日志
        bool InitLogger( const std::string& file );

        // 设置日志登录
        void SetLogLevel( uint32 level );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建日志
        KFSpdLog* NewLogger( uint64 id, const std::string& name, const std::string& file_name );

        // 销毁日志
        void DeleteLogger( uint64 id );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        void RegisterLogFunction( T* object, void ( T::*handle )( uint32, const std::string& ) )
        {
            KFLogFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            SetLogFunction( function );
        }

        template<class T>
        void UnRegisterLogFunction( T* object )
        {
            KFLogFunction function = nullptr;
            SetLogFunction( function );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 打印日志
        template<typename... P>
        void Log( uint32 level, const char* function, uint32 line, const std::string& my_fmt, P&& ... args )
        {
            if ( _kf_logger == nullptr || level < _log_level )
            {
                return;
            }

            static const std::string _function_line_fmt = "[{}:{}] ";

            auto new_fmt = _function_line_fmt + my_fmt;
            auto content = __FORMAT__( new_fmt, function, line, std::forward<P>( args )... );
            _log_function( level, content );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 打印堆栈
        void LogStack( const std::string& error );
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

        // 日志配置
        KFLoggerConfig* _logger_config = nullptr;

        // 创建的log列表
        std::unordered_map< uint64, KFSpdLog* > _create_loggers;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    namespace KFLogEnum
    {
        enum MyEnum
        {
            ////////////////////////////////////////////////////////////////////////
            // log level
            Trace = 0,		// 调试
            Debug = 1,		// 调试
            Info = 2,		// 普通
            Warn = 3,		// 警告
            Error = 4,		// 普通错误
            Critical = 5,	// 致命错误
            Off = 6,		// 程序关闭
            ////////////////////////////////////////////////////////////////////////
        };
    };

#ifdef __KF_DEBUG__
#define __LOG_TRACE__( my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Trace, __FUNC_LINE__, my_fmt, ##__VA_ARGS__ )
#define __LOG_TRACE_FUNCTION__( function, line, my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Trace, function, line, my_fmt, ##__VA_ARGS__ )
#else
#define __LOG_TRACE__( my_fmt, ...)
#define __LOG_TRACE_FUNCTION__( function, line, my_fmt, ...)
#endif // __KF_DEBUG__

#ifdef __KF_DEBUG__
#define __LOG_DEBUG__( my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Debug, __FUNC_LINE__, my_fmt, ##__VA_ARGS__ )
#define __LOG_DEBUG_FUNCTION__( function, line, my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Debug, function, line, my_fmt, ##__VA_ARGS__ )
#else
#define __LOG_DEBUG__( my_fmt, ...)
#define __LOG_DEBUG_FUNCTION__( function, line, my_fmt, ...)
#endif // __KF_DEBUG__

#define __LOG_INFO__( my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Info, __FUNC_LINE__, my_fmt, ##__VA_ARGS__ )
#define __LOG_INFO_FUNCTION__( function, line, my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Info, function, line, my_fmt, ##__VA_ARGS__ )

#define __LOG_WARN__( my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Warn, __FUNC_LINE__, my_fmt, ##__VA_ARGS__ )
#define __LOG_WARN_FUNCTION__( function, line, my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Warn, function, line, my_fmt, ##__VA_ARGS__ )

#define __LOG_ERROR__( my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Error, __FUNC_LINE__, my_fmt, ##__VA_ARGS__ )
#define __LOG_ERROR_FUNCTION__( function, line, my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Error, function, line, my_fmt, ##__VA_ARGS__ )

#define __LOG_CRITICAL__( my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Critical, __FUNC_LINE__, my_fmt, ##__VA_ARGS__ )
#define __LOG_CRITICAL_FUNCTION__( function, line, my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Critical, function, line, my_fmt, ##__VA_ARGS__ )

#define __LOG_OFF__( my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Off, __FUNC_LINE__, my_fmt, ##__VA_ARGS__ )
#define __LOG_OFF_FUNCTION__( function, line, my_fmt, ...) \
    KFLogger::Instance()->Log( KFLogEnum::Off, function, line, my_fmt, ##__VA_ARGS__ )
}

#endif