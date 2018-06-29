#ifndef __KF_CLUSTER_NODE_MODULE_H__
#define __KF_CLUSTER_NODE_MODULE_H__

/************************************************************************
//    @Moudle			:    集群节点模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-18
************************************************************************/

#include "KFrame.h"
#include "KFClusterShardInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFConHash/KFConHash.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////
    class KFClusterShardModule : public KFClusterShardInterface
    {
    public:
        KFClusterShardModule();
        ~KFClusterShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 发型消息到代理服务器
        virtual void SendMessageToProxy( uint32 msgid, const char* data, uint32 length );
        virtual void SendMessageToProxy( uint32 msgid, google::protobuf::Message* message );

        // 发型消息到代理服务器
        virtual bool SendMessageToProxy( uint32 handleid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendMessageToProxy( uint32 handleid, uint32 msgid, google::protobuf::Message* message );

        // 发型消息到代理服务器
        virtual bool SendMessageToClient( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendMessageToClient( const KFGuid& kfguid, uint32 msgid, google::protobuf::Message* message );


        // 发型消息到代理服务器
        virtual bool SendMessageToClient( uint32 clientid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendMessageToClient( uint32 clientid, uint32 msgid, google::protobuf::Message* message );


        // 发型消息到代理服务器
        virtual bool SendMessageToClient( uint32 proxyid, uint32 clientid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendMessageToClient( uint32 proxyid, uint32 clientid, uint32 msgid, google::protobuf::Message* message );

        // 发送消息到玩家
        virtual bool SendMessageToPlayer( uint32 clientid, uint32 playerid, uint32 msgid, google::protobuf::Message* message );

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 注册对象映射
        virtual void AddObjectToProxy( uint64 objectid );
        virtual void AddObjectToProxy( uint32 proxyid, const std::list< uint64 >& objectlist );

        // 删除对象映射
        virtual void RemoveObjectToProxy( uint64 objectid );
        virtual void RemoveObjectToProxy( const std::list< uint64 >& objectlist );
    protected:
        // 查找路由信息
        uint32 FindProxyId( uint32 clientid );

    protected:
        // 注册路由信息
        __KF_MESSAGE_FUNCTION__( HandleClusterClientListReq );

    protected:
        // 丢失连接
        __KF_SERVER_LOST_FUNCTION__( OnServerLostHandle );

    protected:
        // 客户端路由信息
        std::map< uint32, uint32 > _proxy_client_list;
    };
}

#endif