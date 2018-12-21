#ifndef __KF_IP_ADDRESS_CONFIG_H__
#define __KF_IP_ADDRESS_CONFIG_H__

#include "KFIpAddressInterface.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFIpAddressConfig : public KFConfig, public KFSingleton< KFIpAddressConfig >
    {
    public:
        KFIpAddressConfig() = default;
        ~KFIpAddressConfig() = default;

        bool LoadConfig();

        // 查找连接
        const KFIpAddress* FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid );
        void FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid, IpAddressList& outlist );

        // 修改Zone
        void SetZoneIpAddress( const std::string& ip );

        // 修改appid
        void SetMasterAppId( uint32 appchannel, uint32 zoneid );

    public:
        // 配置的连接地址
        std::vector< KFIpAddress > _ip_address_list;

        // 认证地址
        std::string _auth_url;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_ip_config = KFIpAddressConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif