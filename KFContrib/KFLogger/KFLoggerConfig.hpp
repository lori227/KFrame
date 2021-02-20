#ifndef __KF_LOGGER_CONFIG_H__
#define __KF_LOGGER_CONFIG_H__

#include "KFrame.h"

namespace KFrame
{
    namespace KFSinkEnum
    {
        enum MyEnum
        {
            //log split type
            DateAndHour = 1,
            StepFile = 2,
            GLogFile = 3,
        };
    }

    class KFLoggerSetting
    {
    public:
        // output path
        std::string _output_path;

        // file name
        std::string _file_name;

        // split
        std::string _split;

        // sink type
        uint32 _sink_type = KFSinkEnum::DateAndHour;

        // log message queue size( >0 use thread pool )
        uint32 _queue_count = 1024u;

        // sink step seconds
        uint32 _step_seconds = 3600u;

        // sink max size( default 1g )
        uint32 _max_log_size = 1024u * 1024u * 1024u;

        // log pattern
        std::string _pattern;

        // show in console
        bool _console = true;

        // 打印等级
        uint32 _level = 1u;
    };
    //////////////////////////////////////////////////////////////////////////////
    class KFLoggerConfig
    {
    public:
        KFLoggerConfig() = default;
        ~KFLoggerConfig() = default;

        // 加载日志配置
        void LoadConfig( const std::string& file );

        // 查找启动配置
        std::shared_ptr<const KFLoggerSetting> FindSetting( const std::string& name );
    public:
        // 配置列表
        KFHashMap< std::string, KFLoggerSetting > _settings;

        // 默认的日志
        std::string _default_log_name;
    };
}

#endif
