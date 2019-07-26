#ifndef __KF_NET_CONFIG_H__
#define __KF_NET_CONFIG_H__

#include "KFrame.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFTcpSetting
    {
    public:
        std::string _app_name;
        std::string _app_type;
        std::string _local_ip = "0.0.0.0";

        uint32 _port = 0u;
        uint32 _port_type = 0u;
        uint32 _time_out = 20;
        uint32 _max_connection = 20000;
        uint32 _max_queue_size = 10000;
        uint32 _compress_level = 0u;
        uint32 _message_type = KFMessageEnum::Server;
    };

    class KFTcpServerConfig : public KFConfig, public KFInstance< KFTcpServerConfig >
    {
    public:
        KFTcpServerConfig()
        {
            _file_name = "tcp";
        }

        // 加载配置文件
        void LoadConfig( const std::string& file, uint32 loadmask );

        // 查找TcpData
        KFTcpSetting* FindTcpSetting( const std::string& appname, const std::string& apptype );

    public:
        // 服务器列表
        std::vector < KFTcpSetting > _tcp_setting_list;
    };
}

#endif
