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
#include "KFClusterProxy/KFClusterProxyInterface.h"

namespace KFrame
{
    class KFRouteProxyModule : public KFRouteProxyInterface
    {
    public:
        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 丢失连接
        __KF_NET_EVENT_FUNCTION__( OnServerLostClient );

        // 发现连接
        __KF_NET_EVENT_FUNCTION__( OnClientConnectServer );

        // 同步对象
        __KF_MESSAGE_FUNCTION__( HandleRouteSyncObjectToProxyReq, KFMsg::S2SRouteSyncObjectToProxyReq );

        // 添加对象
        __KF_MESSAGE_FUNCTION__( HandleRouteAddObjectToProxyReq, KFMsg::S2SRouteAddObjectToProxyReq );

        // 删除对象
        __KF_MESSAGE_FUNCTION__( HandleRouteRemoveObjectToProxyReq, KFMsg::S2SRouteRemoveObjectToProxyReq );

        // 转发消息到指定服务器
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToServerReq, KFMsg::S2SRouteMessageToServerReq );

        // 转发消息到指定玩家
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToEntityReq, KFMsg::S2SRouteMessageToEntityReq );
    };
}



#endif