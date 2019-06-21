#ifndef __KF_IP_ADDRESS_CONFIG_H__
#define __KF_IP_ADDRESS_CONFIG_H__

#include "KFIpAddressInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFIpAddressConfig : public KFConfig, public KFSingleton< KFIpAddressConfig >
    {
    public:
        KFIpAddressConfig( const std::string& file )
            : KFConfig( file )
        {
        }

        void LoadConfig( const std::string& file );

        // 查找连接
        const KFIpAddress* FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid );
        void FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid, IpAddressList& outlist );

    public:
        // 配置的连接地址
        std::unordered_map< uint64, KFIpAddress > _ip_address_list;

        // 认证地址
        std::string _auth_url;

        // log地址
        std::string _log_url;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_ip_config = KFIpAddressConfig::Instance( "ip.setting" );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif