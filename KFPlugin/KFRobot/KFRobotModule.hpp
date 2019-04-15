#ifndef __KF_ROBOT_MODULE_H__
#define __KF_ROBOT_MODULE_H__

/************************************************************************
//    @Module			:    机器人功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-3-19
************************************************************************/

#include "KFRobot.hpp"
#include "KFRobotConfig.hpp"
#include "KFRobotInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFNetwork/KFNetClientEngine.h"

namespace KFrame
{
    class KFRobotModule : public KFRobotInterface
    {
    public:
        KFRobotModule() = default;
        ~KFRobotModule() = default;

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();
        virtual void OnceRun();
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 创建机器人定时器
        __KF_TIMER_FUNCTION__( OnTimerCreateRobot );

        // 连接成功
        __KF_NET_EVENT_FUNCTION__( OnClientConnectGate );

        // 断开连接
        __KF_NET_EVENT_FUNCTION__( OnClientLostGate );

        // 关闭连接
        __KF_NET_EVENT_FUNCTION__( OnClientShutdownGate );

        // 处理消息
        void HandleNetMessage( const Route& route, uint32 msgid, const char* data, uint32 length );

        // 处理机器人命令
        void ProcessRobotCommand( const VectorString& params );

    protected:
        // 处理错误码
        __KF_MESSAGE_FUNCTION__( HandleResultDisplay );

        // 进入游戏
        __KF_MESSAGE_FUNCTION__( HandleLoginAck );

        // 显示奖励
        __KF_MESSAGE_FUNCTION__( HandleShowElement );

        // 添加属性
        __KF_MESSAGE_FUNCTION__( HandleSyncAddData );

        // 更新属性
        __KF_MESSAGE_FUNCTION__( HandlesyncUpdateData );

        // 删除属性
        __KF_MESSAGE_FUNCTION__( HandlesyncRemoveData );


    private:
        // 机器人序列号
        uint64 _robot_serial = 0u;

        // 机器人列表
        KFHashMap< uint64, uint64, KFRobot > _robots;

        // 客户端引擎
        KFNetClientEngine* _net_client;

        // 处理器函数
        KFBind< uint32, uint32, KFMessageFunction > _message_function;
    };
}

#endif