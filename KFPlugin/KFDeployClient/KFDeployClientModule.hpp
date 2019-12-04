#ifndef __KF_DEPLOY_CLIENT_MODULE_H__
#define __KF_DEPLOY_CLIENT_MODULE_H__


/************************************************************************
//    @Module			:    部署Client
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-2
************************************************************************/

#include "KFDeployClientInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    // 命令属性
    class KFDeployCommand
    {
    public:
        KFBind< std::string, const std::string&, KFDeployFunction > _functions;
    };

    class KFDeployClientModule : public KFDeployClientInterface
    {
    public:
        KFDeployClientModule() = default;
        ~KFDeployClientModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void ShutDown();

    protected:
        virtual void AddFunction( const std::string& command, const std::string& module, KFDeployFunction& function );
        virtual void RemoveFunction( const std::string& command, const std::string& module );

        // 部署命令
        void DeployCommand( const std::string& command, const std::string& value, const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid );

        // 判断是不是自己
        bool IsSelfServer( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid );

        // 关闭服务器
        void ShutDownServer( uint32 delaytime );
        void OnShutDownServerCommand( const std::string& param );

    protected:
        // 关闭服务器
        __KF_TIMER_FUNCTION__( OnTimerShutDownServer );

        // 连接成功
        __KF_NET_EVENT_FUNCTION__( OnClientConnectAgent );

        // 断开连接
        __KF_NET_EVENT_FUNCTION__( OnClientLostAgent );

        // 发送心跳消息
        __KF_TIMER_FUNCTION__( OnTimerSendHeartbeatToAgent );
    protected:
        // 关闭服务器
        __KF_MESSAGE_FUNCTION__( HandleDeployCommandToClientReq );

    private:
        // agentid
        uint64 _agent_id = 0;

        // 命令回调函数
        KFHashMap< std::string, const std::string&, KFDeployCommand > _command_data;
    };
}

#endif