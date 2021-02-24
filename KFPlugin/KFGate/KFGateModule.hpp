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
#include "KFTimer/KFTimerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFDeployClient/KFDeployClientInterface.h"
#include "KFDirClient/KFDirClientInterface.h"

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
        __KF_NET_EVENT_FUNCTION__( OnClientConnectionServer );

        // 断开连接
        __KF_NET_EVENT_FUNCTION__( OnClientLostServer );

        // 玩家掉线
        __KF_NET_EVENT_FUNCTION__( OnPlayerDisconnection );

        // 发送消息到客户端
        __KF_FORWARD_MESSAGE_FUNCTION__( ForwardToClient );

        // 发送消息到游戏服务器
        __KF_FORWARD_MESSAGE_FUNCTION__( TranspondToGame );

        // 更新在线玩家数量
        __KF_TIMER_FUNCTION__( OnTimerUpdateOnlineToDir );

        // 服务器关闭命令
        __KF_DEPLOY_FUNCTION__( OnDeployShutDownServer );

        // 玩家掉线定时器
        __KF_TIMER_FUNCTION__( OnTimerPlayerDisconnetion );
    protected:
        // 登录验证
        __KF_MESSAGE_FUNCTION__( HandleLoginReq, KFMsg::MsgLoginReq );

        // 登出游戏
        __KF_MESSAGE_FUNCTION__( HandleLogoutReq, KFMsg::MsgLogoutReq );
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        // 验证结果
        __KF_MESSAGE_FUNCTION__( HandleLoginToGateAck, KFMsg::S2SLoginToGateAck );

        // 重新登录
        __KF_MESSAGE_FUNCTION__( HandleReLoginToGateAck, KFMsg::S2SReLoginToGateAck );

        // 踢出玩家
        __KF_MESSAGE_FUNCTION__( HandleKickPlayerToGateReq, KFMsg::S2SKickPlayerToGateReq );

        // 处理登录回馈
        __KF_MESSAGE_FUNCTION__( HandleEnterToGateAck, KFMsg::S2SEnterToGateAck );

        // 处理消息广播
        __KF_MESSAGE_FUNCTION__( HandleBroadcastToGateReq, KFMsg::S2SBroadcastToGateReq );
        __KF_MESSAGE_FUNCTION__( HandleBroadcastToServerReq, KFMsg::S2SBroadcastToServerReq );

    protected:
        // 登录到登录服务器
        void LoginToLogin( uint64 sessionid, uint64 account_id, const std::string& token );

        // 发送登录失败消息
        void SendLoginAckMessage( uint64 sessionid, uint32 result, uint64 bantime );

        void AddRole( KFRole* role );
        void RemoveRole( uint64 id );
        void RemoveRole( KFRole* role );
        KFRole* FindRole( uint64 id );
        KFRole* FindRole( const std::string& token );

    private:
        // 服务器关闭
        bool _is_server_shutdown = false;

        // 玩家列表
        KFHashMap< uint64, KFRole > _kf_role_list;
        std::unordered_map< std::string, KFRole* > _token_list;

        // login服务器列表
        KFHash _login_conhash;
        uint64 _login_server_id = _invalid_int;

        // 广播的序号
        UInt64Map _broadcast_list;
    };
}

#endif