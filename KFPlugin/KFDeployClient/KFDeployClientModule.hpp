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
        KFMapFunction< std::string, KFDeployFunction > _functions;
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

        // 调用部署命令
        void CallDeployFunction( const std::string& command, const std::string& value );
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
        __KF_MESSAGE_FUNCTION__( HandleDeployCommandToClientReq, KFMsg::S2SDeployCommandToClientReq );

    protected:
        // 关闭服务器
        __KF_DEPLOY_FUNCTION__( OnDeployShutDownServer );

        // 修改日志等级
        __KF_DEPLOY_FUNCTION__( OnDeployLogLevel );

        // 是否打印内存日志
        __KF_DEPLOY_FUNCTION__( OnDeployLogMemory );

        // 加载配置文件
        __KF_DEPLOY_FUNCTION__( OnDeployLoadConfig );

        // 加载插件
        __KF_DEPLOY_FUNCTION__( OnDeployLoadPlugin );

        // 关闭, 开启消息
        __KF_DEPLOY_FUNCTION__( OnDeployMessageClose );
        __KF_DEPLOY_FUNCTION__( OnDeployMessageOpen );

        // 关闭, 开启模块
        __KF_DEPLOY_FUNCTION__( OnDeployModuleClose );
        __KF_DEPLOY_FUNCTION__( OnDeployModuleOpen );

    private:
        // agentid
        uint64 _agent_id = 0;
        std::string _agent_ip;
        uint32 _agent_port = 0u;

        // 命令回调函数
        KFHashMap< std::string, KFDeployCommand > _command_data;
    };
}

#endif