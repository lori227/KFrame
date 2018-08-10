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
        KFIpAddressConfig();
        ~KFIpAddressConfig();

        bool LoadConfig( const char* file );

        // 查找连接
        const KFIpAddress* FindIpAddress( const std::string& appname, const std::string& apptype, uint32 appid );
        void FindIpAddress( const std::string& appname, const std::string& apptype, uint32 appid, IpAddressList& outlist );

        // 查找平台地址
        const std::string& FindPlatformAddress( uint32 id );

        // 修改Zone
        void SetZoneIpAddress( const std::string& ip );

    protected:
        // 配置的连接地址
        std::vector< KFIpAddress > _ip_address_list;

        // 平台地址
        KFConHash _platform_hash;
        std::map< uint32, std::string > _platform_address;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_ip_config = KFIpAddressConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif