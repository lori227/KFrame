#ifndef __KF_IP_ADDRESS_MODULE_H__
#define __KF_IP_ADDRESS_MODULE_H__

/************************************************************************
//    @Module			:    网络地址配置
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-4
************************************************************************/

#include "KFrame.h"
#include "KFIpAddressInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFIpAddressModule : public KFIpAddressInterface
    {
    public:
        KFIpAddressModule() = default;
        ~KFIpAddressModule() = default;

        // 初始化
        virtual void InitModule();
        virtual void AfterLoad();

        // 关闭
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 获得本机局域网ip
        virtual const std::string& GetLocalIp();

        // 获得本机外网ip
        virtual const std::string& GetInteranetIp();

        // 计算监听端口
        virtual uint32 CalcListenPort( uint32 type, uint32 port, uint64 appid );

        // 查找ip地址
        virtual const KFIpAddress* FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid );
        virtual void FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid, IpAddressList& outlist );

        // 修改小区master地址
        virtual void SetZoneIpAddress( const std::string& ip );

        // 获得auth地址
        virtual const std::string& GetAuthUrl();

        // 获得log地址
        virtual const std::string& GetLogUrl();
    protected:
#if __KF_SYSTEM__ == __KF_WIN__
        std::string GetWinLocalIp();
#else
        std::string GetLinuxLocalIp();
#endif
    protected:
        // 内网ip
        std::string _local_ip;

        // 外网ip
        std::string _interane_ip;
    };
}

#endif