#ifndef __KF_IP_ADDRESS_INTERFACE_H__
#define __KF_IP_ADDRESS_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
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
        virtual const KFIpAddress* FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid ) = 0;
        virtual void FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid, IpAddressList& outlist ) = 0;

        // 计算监听端口
        virtual uint32 CalcListenPort( uint32 type, uint32 port, uint64 appid ) = 0;

        // 获得auth地址
        virtual const std::string& GetAuthUrl() = 0;

        // 获得log地址
        virtual const std::string& GetLogUrl() = 0;

        // 集群认证数据
        virtual const std::string& GetClusterName() = 0;
        virtual const std::string& GetClusterKey() = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_ip_address, KFIpAddressInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif