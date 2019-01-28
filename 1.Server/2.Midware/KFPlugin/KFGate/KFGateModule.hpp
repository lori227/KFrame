#ifndef __KF_GATE_MODULE_H__
#define __KF_GATE_MODULE_H__

/************************************************************************
//    @Module			:    网关功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFRole.hpp"
#include "KFGateInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFDeployClient/KFDeployClientInterface.h"

namespace KFrame
{
    class KFGateModule : public KFGateInterface
    {
    public:
        KFGateModule() = default;
        ~KFGateModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 连接成功
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectionLogin );

        // 断开连接
        __KF_CLIENT_LOST_FUNCTION__( OnClientLostLogin );

        // 玩家掉线
        __KF_SERVER_LOST_FUNCTION__( OnPlayerDisconnection );

        // 发送消息到客户端
        __KF_TRANSMIT_MESSAGE_FUNCTION__( SendToClient );

        // 发送消息到游戏服务器
        __KF_TRANSMIT_MESSAGE_FUNCTION__( SendMessageToGame );

        // 更新在线玩家数量
        __KF_TIMER_FUNCTION__( OnTimerUpdateOnlineToAuth );

        // 服务器关闭命令
        __KF_DEPLOY_COMMAND_FUNCTION__( OnDeployShutDownServer );
    protected:
        // 登录验证
        __KF_MESSAGE_FUNCTION__( HandleLoginReq );

        // 登出游戏
        __KF_MESSAGE_FUNCTION__( HandleLogoutReq );

        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        // 验证结果
        __KF_MESSAGE_FUNCTION__( HandleLoginToGateAck );

        // 处理消息广播
        __KF_MESSAGE_FUNCTION__( HandleBroadcastMessageReq );

        // 踢出玩家
        __KF_MESSAGE_FUNCTION__( HandleKickPlayerToGateReq );

        // 处理登录回馈
        __KF_MESSAGE_FUNCTION__( HandleEnterToGateAck );



    protected:
        // 发送登录失败消息
        void SendLoginAckMessage( uint64 sessionid, uint32 result, uint64 bantime );

    private:
        // 关闭登录
        bool _is_login_close = false;

        // login服务器列表
        KFConHash _kf_login_conhash;

        // 玩家列表
        KFMap< uint64, uint64, KFRole > _kf_role_list;
    };
}

#endif