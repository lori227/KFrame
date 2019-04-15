#ifndef __KF_LOGIN_MODULE_H__
#define __KF_LOGIN_MODULE_H__

/************************************************************************
//    @Module			:    登录服务器模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-4
************************************************************************/

#include "KFLoginInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFZone/KFZoneInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFLoginModule : public KFLoginInterface
    {
    public:
        KFLoginModule() = default;
        ~KFLoginModule() = default;

        // 刷新
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 发送消息到世界服务器
        bool SendToWorld( uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到Gate服务器
        bool SendToGate( uint64 gateid, uint32 msgid, ::google::protobuf::Message* message );

    protected:
        // 连接成功
        __KF_NET_EVENT_FUNCTION__( OnClientConnectionWorld );

        // 断开连接
        __KF_NET_EVENT_FUNCTION__( OnClientLostWorld );

        // gate断开
        __KF_NET_EVENT_FUNCTION__( OnServerLostGate );

        // 平台验证回调
        __KF_HTTP_CALL_BACK_FUNCTION__( OnHttpAuthLoginVerifyCallBack );

        // 注册小区信息
        __KF_TIMER_FUNCTION__( OnTimerZoneRegister );

    protected:
        // 更新小区登录信息
        __KF_MESSAGE_FUNCTION__( HandleUpdateZoneToLoginReq );

        // 登录验证请求
        __KF_MESSAGE_FUNCTION__( HandleLoginToLoginReq );

        // 登录验证结果
        __KF_MESSAGE_FUNCTION__( HandleLoginToLoginAck );

    protected:
        // 发送登录验证结果消息
        void SendLoginAckToGate( uint32 result, uint64 gateid, uint64 sessionid, uint64 accountid, uint64 bantime );

        // 发送到world服务器
        void LoginToWorld( const KFMsg::PBLoginData* pblogin );

    private:
        // 世界服务器id
        KFConHash _world_hash;
        uint64 _world_server_id = _invalid_int;

        // 保存gate负载信息
        KFMsg::PBZoneData _zone_data;

        // redis
        KFRedisDriver* _login_redis = nullptr;
    };
}

#endif