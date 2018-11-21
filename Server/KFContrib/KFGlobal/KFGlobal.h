#ifndef __KF_GOLBAL_H__
#define __KF_GOLBAL_H__

#include "KFSpdLog.h"
#include "spdlog/spdlog.h"
#include "KFVersion.h"

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

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 全局更新
        void RunUpdate();

        // 加载版本
        bool LoadVersion( const std::string& file );

        // 判断版本
        bool CheckVersionCompatibility( const std::string& version );

        // 版本号
        const std::string& GetVersion();

        // 随机数
        uint32 RandRatio( uint32 ratio );

        // 范围内随机
        uint32 RandInRange( uint32 min, uint32 max, uint32 base );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化日志
        void InitLogger( std::string& strtype );

        // 初始化类型
        void InitNetService( std::string& strtype );

        // 判断渠道和服务类型
        bool CheckChannelService( uint32 channel, uint32 service );

        // 设置日志登录
        void SetLogLevel( uint32 level );

        template< class T >
        void RegisterRemoteLogFunction( T* object, bool ( T::*handle )( uint32, const std::string& ) )
        {
            KFLogFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            SetRemoteLogFunction( function );
        }

        // 打印日志
        template<typename... P>
        void Log( uint32 level, const char* function, uint32 line, const std::string& myfmt, P&& ... args )
        {
            if ( _logger == nullptr || level < _log_level )
            {
                return;
            }

            static const std::string _function_line_fmt = "[{}:{}] ";

            auto newfmt = _function_line_fmt + myfmt;
            auto content = __FORMAT__( newfmt, function, line, std::forward<P>( args )... );
            Log( level, content );
        }

    protected:
        KFGlobal();
        // 接口
        static KFGlobal* _kf_global;

        // 打印日志
        void Log( uint32 level, const std::string& content );

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

        // 网络类型( 内外网)
        uint32 _net_type;

        // 运行类型( 正式 测试 )
        uint32 _service_type;

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
        // 版本
        KFVersion* _version;

        // log 指针
        KFLogger* _logger;

        // log打印级别
        uint32 _log_level;

        // 单线程随机类
        KFRand* _rand;

    };
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
#define __LOG_TRACE__( myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::trace, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_TRACE_FUNCTION__( function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::trace, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_DEBUG__( myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::debug, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_DEBUG_FUNCTION__( function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::debug, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_INFO__( myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::info, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_INFO_FUNCTION__( function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::info, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_WARN__( myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::warn, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_WARN_FUNCTION__( function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::warn, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_ERROR__( myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::err, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_ERROR_FUNCTION__( function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::err, function, line, myfmt, ##__VA_ARGS__ )

#define __LOG_CRITICAL__( myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::critical, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_CRITICAL_FUNCTION__( function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::critical, function, line, myfmt, ##__VA_ARGS__ )

    // 本地日志
#define __LOG_LOCAL__( myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::off, __FUNC_LINE__, myfmt, ##__VA_ARGS__ )
#define __LOG_LOCAL_FUNCTION__( function, line, myfmt, ...) \
    KFGlobal::Instance()->Log( spdlog::level::off, function, line, myfmt, ##__VA_ARGS__ )

}

#endif