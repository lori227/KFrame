#ifndef __KF_IP_ADDRESS_INTERFACE_H__
#define __KF_IP_ADDRESS_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFIpAddressInterface : public KFModule
    {
    public:
        /////////////////////////////////////////////////////////////////////////
        // 获得本机局域网ip
        virtual const std::string& GetLocalIp() = 0;

        // 获得本机外网ip
        virtual const std::string& GetInteranetIp() = 0;
        // 计算监听端口
        virtual uint32 CalcListenPort( uint32 type, uint32 port, uint64 appid ) = 0;
        /////////////////////////////////////////////////////////////////////////
        // 获得log地址
        virtual const std::string& GetLogUrl() = 0;

        // 获得auth地址
        virtual const std::string& GetAuthUrl() = 0;

        // 获得dir地址
        virtual const std::string& GetDirUrl() = 0;

        // 获得pay地址
        virtual const std::string& GetPayUrl() = 0;

        // 获得master ip
        virtual const KFIpAddress* GetMasterIp( const std::string& appname, uint32 zoneid ) = 0;

        // 查询master列表
        virtual const std::list< KFIpAddress >& GetMasterList( const std::string& appname, uint32 zoneid ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_ip_address, KFIpAddressInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif