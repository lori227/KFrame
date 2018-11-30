#ifndef __KF_ROUTE_CLIENT_MODULE_H__
#define __KF_ROUTE_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    转发的客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-21
************************************************************************/

#include "KFrame.h"
#include "KFRouteClientInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    class KFRouteClientModule : public KFRouteClientInterface
    {
    public:
        KFRouteClientModule() = default;
        ~KFRouteClientModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 转发消息
        virtual bool SendToRoute( uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToRoute( uint64 serverid, uint64 playerid, uint32 msgid, const char* data, uint32 length );

        // 卸载
        virtual void UnRegisterTransmitFunction();
        virtual void SetTransmitFunction( KFTransmitFunction& function );
    protected:

        // 转发消息
        __KF_MESSAGE_FUNCTION__( HandleTransmitRouteZoneMessageAck );

    private:
        // 连接到RouteProxy
        void OnConnectionRouteCluster( uint64 serverid );
    private:
        // 转发函数
        KFTransmitFunction _kf_transmit_function{ nullptr };
    };
}



#endif