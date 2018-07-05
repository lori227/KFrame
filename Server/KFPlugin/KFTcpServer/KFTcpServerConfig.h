#ifndef __KF_NET_CONFIG_H__
#define __KF_NET_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFTcpSetting
    {
    public:
        KFTcpSetting()
        {
            _port = 0;
            _port_type = 0;
            _max_connection = 20000;
            _max_queue_size = 10000;
            _time_out = 20;
            _local_ip = "0.0.0.0";
        }

        std::string _app_name;
        std::string _app_type;

        std::string _local_ip;
        std::string _interanet_ip;

        uint32 _port_type;
        uint32 _port;
        uint32 _time_out;
        uint32 _max_connection;
        uint32 _max_queue_size;
    };

    class KFTcpServerConfig : public KFConfig, public KFSingleton< KFTcpServerConfig >
    {
    public:
        // 加载配置文件
        bool LoadConfig( const char* file );

        // 查找TcpData
        KFTcpSetting* FindTcpSetting( const std::string& appname, const std::string& apptype );

    public:
        // 服务器列表
        std::vector < KFTcpSetting > _tcp_data_list;
    };
}

////////////////////////////////////////////////////////////////////////////
static auto _kf_server_config = KFrame::KFTcpServerConfig::Instance();
////////////////////////////////////////////////////////////////////////////


#endif
