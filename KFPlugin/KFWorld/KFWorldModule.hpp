#ifndef __KF_WORLD_MODULE_H__
#define __KF_WORLD_MODULE_H__

/************************************************************************
//    @Module			:    世界功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFWorldInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFZConfig/KFZoneConfig.hpp"

namespace KFrame
{
    class KFWorldModule : public KFWorldInterface
    {
    public:
        KFWorldModule() = default;
        ~KFWorldModule() = default;


        // 刷新
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 断开Game
        __KF_NET_EVENT_FUNCTION__( OnServerLostGame );

        // 处理踢人
        __KF_HTTP_FUNCTION__( HandleHttpKickOnline );

        // 注册world服务器
        __KF_TIMER_FUNCTION__( OnTimerWorldRegister );

    protected:
        // 添加Gate信息
        __KF_MESSAGE_FUNCTION__( HandleAddGateToWorldReq );

        // 删除gate信息
        __KF_MESSAGE_FUNCTION__( HandleRemoveGateToWorldReq );

        // 验证登录
        __KF_MESSAGE_FUNCTION__( HandleLoginToWorldReq );

        // 踢人请求
        __KF_MESSAGE_FUNCTION__( HandleKickPlayerToWorldReq );
        __KF_MESSAGE_FUNCTION__( HandleKickPlayerToWorldAck );

        // 玩家进入游戏
        __KF_MESSAGE_FUNCTION__( HandlePlayerEnterToWorldReq );

        // 玩家离开游戏
        __KF_MESSAGE_FUNCTION__( HandlePlayerLeaveToWorldReq );

        // 广播消息
        __KF_MESSAGE_FUNCTION__( HandleBroadcastToGameReq );
        __KF_MESSAGE_FUNCTION__( HandleBroadcastToWorldReq );
        __KF_MESSAGE_FUNCTION__( HandleBroadcastToWorldAck );

    protected:
        // 发送验证结果消息
        void SendLoginAckToLogin( uint32 result, uint64 loginid, uint64 gateid, uint64 accountid, uint64 sessionid );

        // 更新在线状态
        void UpdateOnlineData( uint64 playerid, uint64 gameid );

        // 踢掉在线玩家
        bool KickOnline( uint32 type, uint64 playerid, const char* function, uint32 line );
        void SendKickMessage( uint32 type, uint64 playerid, uint64 gameid );

        // 查找一个可以用的game
        uint64 FindLoginGame( uint64 gateid, uint64 playerid );

        // 查询玩家id
        uint64 QueryCreatePlayerId( uint64 accountid );

        // 广播消息
        void BroadCastToGame( uint32 msgid, const std::string& msgdata, uint32 serial, uint64 worldid );

    private:
        // 游戏服务列表
        KFHashMap< uint64, uint64, KFConHash > _gate_conhash;

        // 广播的序号
        uint32 _broadcast_serial = 0u;

        // redis
        KFRedisDriver* _auth_redis = nullptr;
    };
}

#endif