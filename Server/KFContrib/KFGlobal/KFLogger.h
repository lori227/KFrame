#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "KFInclude.h"
#include "KFDefine.h"
#include "KFUtility/KFMutex.h"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    enum KFLogTypeEnum
    {
        Local = 1,
        Remote = 2,
    };

    class KFLogger
    {
    public:
        KFLogger() = default;
        virtual ~KFLogger() = default;

        // 循环打log
        virtual void RunUpdate() {};

        // 打印日志
        virtual void Log( uint32 loglevel, uint32 category, const std::string& content ) = 0;

        // 初始化
        virtual bool Initialize( const std::string& appname, const std::string& apptype, uint32 appid ) = 0;

        // 设置远程log函数
        virtual void SetRemoteLogFunction( KFLogFunction& function ) {};
    };

    /////////////////////////////////////////////////////////////////////
    class KFSpdLog;
    class KFLocalLogger : public KFLogger
    {
    public:
        KFLocalLogger();
        virtual ~KFLocalLogger();

        // 打印日志
        virtual void Log( uint32 loglevel, uint32 category, const std::string& content );

        // 初始化
        virtual bool Initialize( const std::string& appname, const std::string& apptype, uint32 appid );

    protected:
        // 日志逻辑
        KFSpdLog* _spd_log;
    };

    /////////////////////////////////////////////////////////////////////
    class KFLogData
    {
    public:
        KFLogData()
        {
            _category = 0;
            _log_level = 0;
        }
    public:
        uint32 _category;
        uint32 _log_level;
        std::string _content;
    };


    class KFRemoteLogger : public KFLocalLogger
    {
    public:
        KFRemoteLogger();
        virtual ~KFRemoteLogger();

        // 循环打log
        virtual void RunUpdate();

        // 打印日志
        virtual void Log( uint32 loglevel, uint32 category, const std::string& content );

        // 设置远程log函数
        virtual void SetRemoteLogFunction( KFLogFunction& function );

    protected:
        // 本地日志函数
        KFLogFunction _remote_log_function;

        // 线程锁
        KFMutex _kf_mutex;

        // 缓存的log队列
        std::list< KFLogData* > _log_data_list;
    };
}

#endif
