#ifndef __KF_DIR_CLIENT_INTERFACE_H__
#define __KF_DIR_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDirClientInterface : public KFModule
    {
    public:
        // 注册小区信息到目录服务器
        virtual bool ZoneRegisterToDir() = 0;

        // 注册world到目录服务器
        virtual bool WorldRegisterToDir( uint64 worldid, const std::string& url ) = 0;

        // 更新gate的在线数量
        virtual void UpdateGateToDir( uint64 appid, const std::string& ip, uint32 port, uint32 count, uint32 expiretime ) = 0;

        // 更新小区的负载均衡
        virtual void ZoneBalanceToDir( uint32 zoneid, uint32 count ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_dir_client, KFDirClientInterface );
    ////////////////////////////////////////////////////////////////////////////////
}


#endif