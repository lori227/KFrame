#ifndef __KF_NET_CONFIG_H__
#define __KF_NET_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFListen
    {
    public:
        KFListen()
        {
            _port = 0;
            _type = 0;
            _timeout = 2000;
            _max_connection = 5000;
            _max_queue_size = 10000;
        }

        std::string _local_ip;
        std::string _interanet_ip;

        uint32 _type;
        uint32 _port;
        uint32 _timeout;
        uint32 _max_connection;
        uint32 _max_queue_size;
    };

    class KFTcpServerConfig : public KFConfig, public KFSingleton< KFTcpServerConfig >
    {
    public:

        // 加载配置文件
        bool LoadConfig( const char* file );

    public:
        // 类型
        uint32 _server_type;

        // 服务器监听端口
        KFListen _kf_listen;
    };
}

////////////////////////////////////////////////////////////////////////////
static auto _kf_server_config = KFrame::KFTcpServerConfig::Instance();
////////////////////////////////////////////////////////////////////////////


#endif
