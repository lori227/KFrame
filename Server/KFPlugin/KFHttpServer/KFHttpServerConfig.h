#ifndef __KF_HTTP_SERVER_CONFIG_H__
#define __KF_HTTP_SERVER_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFHttpServerConfig : public KFConfig, public KFSingleton< KFHttpServerConfig >
    {
    public:
        KFHttpServerConfig();

        // 加载配置文件
        bool LoadConfig( const char* file );

    public:
        // 内网ip
        std::string _local_ip;

        // 外网ip
        std::string _interanet_ip;

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
}

////////////////////////////////////////////////////////////////////////////
static auto _kf_http_server_config = KFrame::KFHttpServerConfig::Instance();
////////////////////////////////////////////////////////////////////////////


#endif
