#ifndef __KF_TCP_DISCOVER_MODULE_H__
#define __KF_TCP_DISCOVER_MODULE_H__

/************************************************************************
//    @Module			:    tcp网络发现
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-7
************************************************************************/

#include "KFTcpDiscoverInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"

namespace KFrame
{
    class KFTcpDiscoverModule : public KFTcpDiscoverInterface
    {
    public:
        KFTcpDiscoverModule() = default;
        ~KFTcpDiscoverModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void ShutDown();

    protected:
        // 发现新客户端
        __KF_NET_EVENT_FUNCTION__( OnServerDiscoverClient );

        // 丢失客户端
        __KF_NET_EVENT_FUNCTION__( OnServerLostClient );

        // 连接服务器
        __KF_NET_EVENT_FUNCTION__( OnClientConnectServer );

        // 请求master列表
        __KF_TIMER_FUNCTION__( OnTimerQueryMasterList );

        // 通知发现新的服务器
        __KF_MESSAGE_FUNCTION__( HandleTellDiscoverServerToMaster );

        // 通知丢失服务器
        __KF_MESSAGE_FUNCTION__( HandleTellLostServerToMaster );

        // 通知注册
        __KF_MESSAGE_FUNCTION__( HandleTellRegisterServerToMaster );

        // 服务器列表
        __KF_MESSAGE_FUNCTION__( HandleTellServerListToMaster );
    };
}

#endif