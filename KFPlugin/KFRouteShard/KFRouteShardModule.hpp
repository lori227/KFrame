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
#include "KFRouteService.hpp"
#include "KFRouteShardInterface.h"
#include "KFNetwork/KFNetMessage.hpp"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"

namespace KFrame
{
    class KFMessageData
    {
    public:
        // 添加消息
        void AddNetMessage( uint32 msgid, const char* data, uint32 length );

        // 发送消息
        void SendNetMessage();
    public:
        // 消息列表
        std::list< KFNetMessage* > _messages;
    };

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

        // Route客户端丢失
        __KF_MESSAGE_FUNCTION__( HandleRouteClientLostToShardReq );

        // 同步对象
        __KF_MESSAGE_FUNCTION__( HandleRouteSyncObjectToShardReq );

        // 添加对象
        __KF_MESSAGE_FUNCTION__( HandleRouteAddObjectToShardReq );

        // 删除对象
        __KF_MESSAGE_FUNCTION__( HandleRouteRemoveObjectToShardReq );

    protected:
        // 转发消息
        void SendRouteMessage( uint64 clientid, KFMsg::PBRoute* pbroute, uint32 msgid, const std::string& msgdata );

        // 添加转发失败的消息
        void AddRouteFailedMessage( const std::string& name, uint32 msgid, const char* data, uint32 length );

        // 发送转发失败的消息
        void SendRouteFailedMessage( const std::string& name );
    private:
        // 注册的转发服务
        KFHashMap< std::string, KFRouteService > _route_service_list;

        // 转发失败的消息
        KFHashMap< std::string, KFMessageData > _route_message_list;
    };
}



#endif