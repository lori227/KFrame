#ifndef __KF_IP_ADDRESS_MODULE_H__
#define __KF_IP_ADDRESS_MODULE_H__

/************************************************************************
//    @Module			:    网络地址配置
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-4
************************************************************************/

#include "KFIpConfig.hpp"
#include "KFIpAddressInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFIpAddressModule : public KFIpAddressInterface
    {
    public:
        KFIpAddressModule() = default;
        ~KFIpAddressModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 加载配置
        virtual void AfterLoad();
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 获得本机局域网ip
        virtual const std::string& GetLocalIp();

        // 获得本机外网ip
        virtual const std::string& GetInteranetIp();

        // 计算监听端口
        virtual uint32 CalcListenPort( uint32 type, uint32 port, uint64 appid );

        // 获得log地址
        virtual const std::string& GetLogUrl();

        // 获得auth地址
        virtual const std::string& GetAuthUrl();

        // 获得dir地址
        virtual const std::string& GetDirUrl();

        // 获得pay地址
        virtual const std::string& GetPayUrl();

        // 获得master ip
        virtual const KFIpAddress* GetMasterIp( const std::string& appname, uint32 zoneid );

        // 查询master列表
        virtual const std::list< KFIpAddress >& GetMasterList( const std::string& appname, uint32 zoneid );
    protected:
#if __KF_SYSTEM__ == __KF_WIN__
        std::string GetWinLocalIp();
#else
        std::string GetLinuxLocalIp();
#endif

        // 更新masterip
        __KF_TIMER_FUNCTION__( OnTimerUpdateMasterIp );

    protected:
        // 内网ip
        std::string _local_ip;

        // 外网ip
        std::string _interanet_ip;
    };
}

#endif