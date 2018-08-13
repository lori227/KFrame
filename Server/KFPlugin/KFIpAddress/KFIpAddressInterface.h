#ifndef __KF_IP_ADDRESS_INTERFACE_H__
#define __KF_IP_ADDRESS_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // Ip地址配置
    class KFIpAddress
    {
    public:
        KFIpAddress()
        {
            _app_id = 0;
            _port = 0;
            _port_type = 0;
        }

    public:
        // 服务器名字
        std::string _app_name;

        // 服务器类型
        std::string _app_type;

        // 服务器id
        uint32 _app_id;
        std::string _str_app_id;

        // 服务器ip
        std::string _ip;

        // 端口类型
        uint32 _port_type;

        // 服务器端口
        uint32 _port;
    };
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFIpAddressInterface : public KFModule
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        // 获得本机局域网ip
        virtual const std::string& GetLocalIp() = 0;

        // 获得本机外网ip
        virtual const std::string& GetInteranetIp() = 0;
        /////////////////////////////////////////////////////////////////////////
        // 查找ip地址
        virtual const KFIpAddress* FindIpAddress( const std::string& appname, const std::string& apptype, uint32 appid ) = 0;
        virtual void FindIpAddress( const std::string& appname, const std::string& apptype, uint32 appid, IpAddressList& outlist ) = 0;

        // 计算监听端口
        virtual uint32 CalcTcpListenPort( uint32 type, uint32 port, uint32 appid ) = 0;
        virtual uint32 CalcHttpListenPort( uint32 type, uint32 port, uint32 appid ) = 0;

        // 修改小区master地址
        virtual void SetZoneIpAddress( const std::string& ip ) = 0;

        // 平台访问地址
        virtual const std::string& FindPlatformAddress( uint32 id ) = 0;

    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_ip_address, KFIpAddressInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif