#ifndef __KF_HTTP_SERVER_CONFIG_H__
#define __KF_HTTP_SERVER_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFHttpSetting
    {
    public:
        KFHttpSetting()
        {
            _local_ip = "0.0.0.0";
            _port_type = 0;
            _port = 0;
            _keep_alive = false;
            _max_thread = 4;
            _max_queue = 64;
            _idle_time = 100;
        }

    public:
        // 服务器名字
        std::string _app_name;

        // 服务器类型
        std::string _app_type;

        // 内网ip
        std::string _local_ip;

        // 端口类型
        uint32 _port_type;

        // 端口
        uint32 _port;

        // 是否保持
        bool _keep_alive;

        // 最大线程
        uint32 _max_thread;

        // 最大队列
        uint32 _max_queue;

        // 线程idle时间
        uint32 _idle_time;
    };

    class KFHttpServerConfig : public KFConfig, public KFSingleton< KFHttpServerConfig >
    {
    public:
        // 加载配置文件
        bool LoadConfig( const std::string& file );

        // 查找配置
        KFHttpSetting* FindHttpSetting( const std::string& appname, const std::string& apptype );

    public:
        // 配置列表
        std::vector < KFHttpSetting > _http_setting_list;
    };
}

////////////////////////////////////////////////////////////////////////////
static auto _kf_http_server_config = KFrame::KFHttpServerConfig::Instance();
////////////////////////////////////////////////////////////////////////////


#endif
