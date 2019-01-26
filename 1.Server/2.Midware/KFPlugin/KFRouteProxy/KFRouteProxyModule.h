#ifndef __KF_ROUTE_PROXY_MODULE_H__
#define __KF_ROUTE_PROXY_MODULE_H__

/************************************************************************
//    @Module			:    转发代理模块
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

namespace KFrame
{
    class KFRouteProxyModule : public KFRouteProxyInterface
    {
    public:
        KFRouteProxyModule() = default;
        ~KFRouteProxyModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 丢失连接
        __KF_SERVER_LOST_FUNCTION__( OnServerLostClient );

        // 发现连接
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectServer );

        // 同步对象
        __KF_MESSAGE_FUNCTION__( HandleRouteSyncObjectToProxyReq );

        // 添加对象
        __KF_MESSAGE_FUNCTION__( HandleRouteAddObjectToProxyReq );

        // 删除对象
        __KF_MESSAGE_FUNCTION__( HandleRouteRemoveObjectToProxyReq );
    };
}



#endif