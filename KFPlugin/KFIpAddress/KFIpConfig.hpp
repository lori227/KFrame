#ifndef __KF_IP_ADDRESS_CONFIG_H__
#define __KF_IP_ADDRESS_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFIpConfig : public KFConfig, public KFInstance< KFIpConfig >
    {
    public:
        KFIpConfig()
        {
            _file_name = "ip";
        }

        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

        // 查找vpn ip
        std::tuple<std::string, uint32> FindVPNIpAddress( const std::string& appname, const std::string& apptype, uint32 zoneid );
    public:
        // 获得ip的访问地址
        std::string _dns_get_ip_url;

        // log地址
        std::string _log_url;

        // 认证地址
        std::string _auth_url;

        // dir地址
        std::string _dir_url;

        // dir地址
        std::string _pay_url;

        // vpn 地址
        std::unordered_map<uint32, std::tuple<std::string, uint32>> _vpn_list;
    };
}

#endif