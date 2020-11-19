#ifndef __KF_DIR_CLIENT_MODULE_H__
#define __KF_DIR_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    目录模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFDirClientInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFConfig/KFZoneConfig.hpp"

namespace KFrame
{
    class KFDirClientModule : public KFDirClientInterface
    {
    public:
        KFDirClientModule() = default;
        ~KFDirClientModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 注册消息信息到
        virtual bool ZoneRegisterToDir();

        // 注册world到目录服务器
        virtual bool WorldRegisterToDir( uint64 worldid, const std::string& url );

        // 更新gate的在线数量
        virtual void UpdateGateToDir( uint64 appid, const std::string& ip, uint32 port, uint32 count, uint32 expiretime );

        // 更新小区的负载均衡
        virtual void ZoneBalanceToDir( uint32 zoneid, uint32 count );
    };
}



#endif