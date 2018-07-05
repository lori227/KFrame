#ifndef __KF_DEPLOY_AGENT_MODULE_H__
#define __KF_DEPLOY_AGENT_MODULE_H__


/************************************************************************
//    @Module			:    部署Agent
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-2
************************************************************************/

#include "KFrame.h"
#include "KFDeployAgentInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    class KFLaunchSetting;
    class KFDeployAgentModule : public KFDeployAgentInterface
    {
    public:
        KFDeployAgentModule();
        ~KFDeployAgentModule();

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void ShutDown();
    protected:
        // 连接成功
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectServer );

        // 启动服务器
        __KF_TIMER_FUNCTION__( OnTimerStartupProcess );

    protected:
        // 更新数据到部署服务
        void UpdateProcessToServer( KFLaunchSetting* kfsetting );

        // 启动服务器进程
        void StartupServerProcess( KFLaunchSetting* kfsetting );

        // 检查服务器进程
        void CheckServerProcess( KFLaunchSetting* kfsetting );

        // 绑定继承
        void BindServerProcess();

        // 杀死进程
        void KillServerProcess( uint32 processid );

        // 保存进程信息到文件中
        char* FormatPidFileName( KFLaunchSetting* kfsetting );

        void SaveProcessToFile( KFLaunchSetting* kfsetting );
        void ReadProcessFromFile( KFLaunchSetting* kfsetting );

#if __KF_SYSTEM__ == __KF_WIN__
        // 启动进程
        bool StartupWinProcess( KFLaunchSetting* kfsetting );

        // 检查进程是否存在
        void CheckWinProcess( KFLaunchSetting* kfsetting );

        // 杀死进程
        void KillWinProcess( uint32 processid );
#else
        // 启动进程
        bool StartupLinuxProcess( KFLaunchSetting* kfsetting );

        // 获得linux进程id
        uint32 FindProcessIdByName( KFLaunchSetting* kfsetting, const std::string& startupfile );

        // 检查进程是否存在
        void CheckLinuxProcess( KFLaunchSetting* kfsetting );

        // 杀死进程
        void KillLinuxProcess( uint32 processid );
#endif

    private:
        uint32 _deploy_server_id;
    };
}

#endif