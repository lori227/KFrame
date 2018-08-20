#ifndef __KF_ROUTE_PROXY_MODULE_H__
#define __KF_ROUTE_PROXY_MODULE_H__

/************************************************************************
//    @Module			:    适配器代理模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-21
************************************************************************/

#include "KFrame.h"
#include "KFRouteProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFClusterProxy/KFClusterProxyInterface.h"

namespace KFrame
{
    class KFRouteProxyModule : public KFRouteProxyInterface
    {
    public:
        KFRouteProxyModule();
        ~KFRouteProxyModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 注册游戏区请求
        __KF_MESSAGE_FUNCTION__( HandleRegisterZoneReq );

        // 转发消息
        __KF_MESSAGE_FUNCTION__( HandleTransmitRouteZoneMessageReq );

        // 转发消息
        __KF_MESSAGE_FUNCTION__( HandleTransmitRouteProxyMessageAck );
    protected:
        // 注册到Route shard回调
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectionRouteShard );
    };
}



#endif