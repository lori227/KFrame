#ifndef __KF_ROUTE_CLIENT_MODULE_H__
#define __KF_ROUTE_CLIENT_MODULE_H__

/************************************************************************
//    @Moudle			:    适配器的客户端模块
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
        KFRouteClientModule();
        ~KFRouteClientModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 转发消息
        virtual bool SendMessageToRoute( uint32 serverid, uint32 playerid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendMessageToRoute( uint32 serverid, uint32 playerid, uint32 msgid, const char* data, uint32 length );

        // 卸载
        virtual void UnRegisterTransmitFunction();
        virtual void SetTransmitFunction( KFTransmitFunction& function );
    protected:

        // 转发消息
        __KF_MESSAGE_FUNCTION__( HandleTransmitRouteZoneMessageAck );

    private:
        // 连接到RouteProxy
        void OnConnectionRouteProxy( uint32 serverid );
    private:
        // 转发函数
        KFTransmitFunction _kf_transmit_function;
    };
}



#endif