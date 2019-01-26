#ifndef __KF_WORLD_MODULE_H__
#define __KF_WORLD_MODULE_H__

/************************************************************************
//    @Module			:    世界功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFrame.h"
#include "KFWorldInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFDeployCommand/KFDeployCommandInterface.h"
#include "KFOnline.h"

namespace KFrame
{
    class KFWorldModule : public KFWorldInterface
    {
    public:
        KFWorldModule() = default;
        ~KFWorldModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 断开Game
        __KF_SERVER_LOST_FUNCTION__( OnServerLostGame );

        // 发现Game
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDisCoverGame );

        // 走马灯
        __KF_COMMAND_FUNCTION__( OnCommandMarquee );

        // 系统公告
        __KF_COMMAND_FUNCTION__( OnCommandNotice );

        // 处理踢人
        __KF_HTTP_FUNCTION__( HandleHttpKickOnline );

        // 注册小区信息
        __KF_TIMER_FUNCTION__( OnTimerZoneRegister );

    protected:
        // 验证登录
        __KF_MESSAGE_FUNCTION__( HandleLoginWorldVerifyReq );

        // 在线玩家同步
        __KF_MESSAGE_FUNCTION__( HandleGameSyncOnlineReq );

        // 处理消息转发
        __KF_MESSAGE_FUNCTION__( HandleTransmitToPlayerReq );

        // 处理消息转发
        __KF_MESSAGE_FUNCTION__( HandleTransmitToServerReq );

        // 处理广播消息
        __KF_MESSAGE_FUNCTION__( HandleBroadcastMessageReq );

        // 玩家进入游戏
        __KF_MESSAGE_FUNCTION__( HandlePlayerEnterWorldReq );

        // 玩家离开游戏
        __KF_MESSAGE_FUNCTION__( HandlePlayerLeaveWorldReq );

    protected:
        // 广播消息
        void BroadcastMessageToGame( uint32 msgid, google::protobuf::Message* message );
        void BroadcastMessageToGame( uint32 msgid, const char* data, uint32 length );

        // 发送验证结果消息
        void SendVerifyFailedToLogin( uint32 result, uint64 loginid, uint64 gateid, uint64 accountid, uint64 sessionid );

        // 更新在线状态
        void UpdateOnlineToAuth( uint64 accountid, uint64 playerid, bool online );

        // 踢掉在线玩家
        bool KickOnline( uint64 playerid, const char* function, uint32 line );

        // 发送消息到玩家
        bool SendToOnline( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到游戏服务器
        bool SendToGame( uint64 gameid, uint32 msgid, ::google::protobuf::Message* message );
    private:
        // 游戏服务列表
        KFConHash _kf_game_conhash;

        // 在线玩家列表
        KFMap< uint64, uint64, KFOnline > _kf_online_list;
    };
}

#endif