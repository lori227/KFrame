#ifndef __KF_GOLBAL_H__
#define __KF_GOLBAL_H__

#include "KFLog.h"
#include "spdlog/spdlog.h"

namespace KFrame
{
    class KFRand;
    class KFLogger;

    class KFGlobal
    {
    public:
        ~KFGlobal();

        // 创建
        static void Initialize( KFGlobal* kfglobal );

        // 接口
        static KFGlobal* Instance();

        // 全局更新
        void RunUpdate();

        // 随机数
        uint32 RandRatio( uint32 ratio );

        // 范围内随机
        uint32 RandInRange( uint32 min, uint32 max, uint32 base );


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化日志
        void InitLogger( std::string& strtype );

        // 设置日志登录
        void SetLogLevel( uint32 level );

        template< class T >
        void RegisterRemoteLogFunction( T* object, bool ( T::*handle )( uint32, uint32, const std::string& ) )
        {
            KFLogFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            SetRemoteLogFunction( function );
        }

        // 打印日志
        template<typename... P>
        void Log( uint32 level, uint32 category, const char* function, uint32 line, const std::string& myfmt, P&& ... args )
        {
            if ( _logger == nullptr || level < _log_level )
            {
                return;
            }

            static const std::string _function_line_fmt = "[{}:{}] ";

            auto newfmt = _function_line_fmt + myfmt;
            auto content = __FORMAT__( newfmt, function, line, std::forward<P>( args )... );
            Log( level, category, content );
        }

    protected:
        KFGlobal();
        // 接口
        static KFGlobal* _kf_global;

        // 打印日志
        void Log( uint32 level, uint32 category, const std::string& content );

        // 设置远程日志函数
        void SetRemoteLogFunction( KFLogFunction& function );

    public:
        // 程序运行
        volatile bool _app_run;

        // 现实时间
        uint64 _real_time;

        // 游戏时间
        uint64 _game_time;

        // appid
        uint32 _app_id;
        std::string _str_app_id;

        // 类型
        std::string _app_type;

        // 名字
        std::string _app_name;

        // 分区id
        uint32 _zone_id;

        // 服务渠道
        uint32 _app_channel;

        // 内网ip
        std::string _local_ip;

        // 外网ip
        std::string _interanet_ip;

        // 监听端口
        uint32 _listen_port;

        // 程序标题
        std::string _title_text;

        /////////////////////////////////////////////////////////////////////////////////

    private:
        // log 指针
        KFLogger* _logger;

        // log打印级别
        uint32 _log_level;

        // 单线程随机类
        KFRand* _rand;
    };
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
#define __LOG_TRACE__( category, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::trace, category, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_TRACE_FUNCTION__( category, function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::trace, category, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_DEBUG__( category, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::debug, category, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_DEBUG_FUNCTION__( category, function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::debug, category, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_INFO__( category, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::info, category, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_INFO_FUNCTION__( category, function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::info, category, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_WARN__( category, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::warn, category, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_WARN_FUNCTION__( category, function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::warn, category, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_ERROR__( category, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::err, category, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_ERROR_FUNCTION__( category, function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::err, category, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_CRITICAL__( category, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::critical, category, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_CRITICAL_FUNCTION__( category, function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::critical, category, function, line, myfmt, ##__VA_ARGS__ )

    // 本地日志
#define __LOG_LOCAL__( category, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::off, category, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_LOCAL_FUNCTION__( category, function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::off, category, function, line, myfmt, ##__VA_ARGS__ )

}

#endif