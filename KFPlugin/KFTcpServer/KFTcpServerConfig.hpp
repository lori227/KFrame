#ifndef __KF_NET_CONFIG_H__
#define __KF_NET_CONFIG_H__

#include "KFrame.h"
#include "KFConfig.h"

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
        uint32 _handle_message_count = 20; // 每帧处理消息数量
        uint32 _message_type = KFMessageEnum::Server;
        uint32 _compress_length = 0u;
        bool _open_encrypt = false;
    };

    class KFTcpServerConfig : public KFConfig, public KFInstance< KFTcpServerConfig >
    {
    public:
        KFTcpServerConfig()
        {
            _file_name = "tcp";
        }

        // 加载配置文件
        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

        // 查找TcpData
        KFTcpSetting* FindTcpSetting( const std::string& appname, const std::string& apptype );

    public:
        // 服务器列表
        std::vector < KFTcpSetting > _tcp_setting_list;

        // 压缩类型
        uint32 _compress_type = 0u;

        // 压缩等级
        uint32 _compress_level = 0u;

        // 加密秘钥
        std::string _encrypt_key;
    };
}

#endif
