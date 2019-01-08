#ifndef __KF_ROUTE_SHARD_MODULE_H__
#define __KF_ROUTE_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    转发逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-20
************************************************************************/

#include "KFrame.h"
#include "KFRouteShardInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"

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
        // proxy 断开连接
        __KF_SERVER_LOST_FUNCTION__( OnServerLostRouteProxy );

    protected:
        // 转发消息到所有服务器
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToAllReq );

        // 转发消息到某类型所有服务器
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToNameAllReq );

        // 转发消息到随机服务器
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToNameRandReq );

        // 转发消息到负载最小服务器
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToNameBalanceReq );

        // 转发消息到对象所在服务器
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToNameObjectReq );

        // 转发消息到指定服务器
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToServerReq );

        // 转发消息到指定玩家
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToPlayerReq );

    protected:
        // 转发消息
        void SendRouteMessage( uint64 clientid, uint32 msgid, const std::string& msgdata, uint64 sourceid, uint64 playerid = _invalid_int );

    };
}



#endif