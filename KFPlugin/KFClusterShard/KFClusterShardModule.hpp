#ifndef __KF_CLUSTER_NODE_MODULE_H__
#define __KF_CLUSTER_NODE_MODULE_H__

/************************************************************************
//    @Module			:    集群节点模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-18
************************************************************************/

#include "KFrame.h"
#include "KFClusterShardInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////
    class KFClusterShardModule : public KFClusterShardInterface
    {
    public:
        KFClusterShardModule() = default;
        ~KFClusterShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 发送消息到proxy
        virtual void SendToProxy( uint32 msg_id, google::protobuf::Message* message );
        virtual bool SendToProxy( uint64 handle_id, uint32 msg_id, google::protobuf::Message* message );

        // 发送消息到client
        virtual bool SendToClient( uint32 msg_id, google::protobuf::Message* message );
        virtual bool SendToClient( uint64 clientid, uint32 msg_id, google::protobuf::Message* message );
        virtual bool SendToClient( uint64 proxyid, uint64 clientid, uint32 msg_id, google::protobuf::Message* message );


    protected:
        // 注册路由信息
        __KF_MESSAGE_FUNCTION__( HandleClusterClientDiscoverToShardReq, KFMsg::S2SClusterClientDiscoverToShardReq );

        // 客户端丢失
        __KF_MESSAGE_FUNCTION__( HandleClusterClientLostToShardReq, KFMsg::S2SClusterClientLostToShardReq );

        // 丢失连接
        __KF_NET_EVENT_FUNCTION__( OnServerLostHandle );

    protected:
        // 查找路由信息
        uint64 FindProxyId( uint64 clientid );

    protected:
        // 客户端路由信息
        UInt64Map _proxy_client_list;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif