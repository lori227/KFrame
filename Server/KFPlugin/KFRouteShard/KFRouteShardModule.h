#ifndef __KF_ROUTE_SHARD_MODULE_H__
#define __KF_ROUTE_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    适配器逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-20
************************************************************************/

#include "KFrame.h"
#include "KFRouteShardInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    class KFRouteShardModule : public KFRouteShardInterface
    {
    public:
        KFRouteShardModule() = default;
        ~KFRouteShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理 route proxy注册
        __KF_MESSAGE_FUNCTION__( HandleRegisterRouteProxyReq );

        // 处理消息转发
        __KF_MESSAGE_FUNCTION__( HandleTransmitRouteProxyMessageReq );
    protected:
        // route proxy 断开连接
        __KF_SERVER_LOST_FUNCTION__( OnServerLostRouteProxy );
    };
}



#endif