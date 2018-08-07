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
#include "KFFtp/KFFtpInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFDeployTask
    {
    public:
        KFDeployTask()
        {
            _app_id = _invalid_int;
            _zone_id = _invalid_int;
            _start_time = 0;
        }

    public:
        // 部署命令
        std::string _command;

        // 数值
        std::string _value;

        // appname
        std::string _app_name;

        // apptype
        std::string _app_type;

        // appid
        uint32 _app_id;

        // zoneid
        uint32 _zone_id;

        // 开始时间
        uint64 _start_time;
    };

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

        // 检查任务完成
        __KF_TIMER_FUNCTION__( OnTimerCheckTaskFinish );

    protected:
        // 启动服务器
        __KF_MESSAGE_FUNCTION__( HandleDeployCommandReq );

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
        std::string FormatPidFileName( KFLaunchSetting* kfsetting );

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

    protected:
        // 添加部署任务
        void AddDeployTask( const std::string& command, const std::string& value, const std::string& appname, const std::string& apptype, uint32 appid, uint32 zoneid );

        // 开始任务
        void StartDeployTask();
        void StartKillServerTask();
        void StartShutDownServerTask();
        void StartStartupServerTask();
        void StartUpdateServerTask();

        // 检查任务完成
        bool CheckShutDownServerTaskFinish();
        bool CheckStartupServerTaskFinish();
        bool CheckUpdateServerTaskFinish();

        // ftp下载回调
        void OnFtpDownLoadCallBack( uint32 objectid, const std::string& apppath, bool ftpok );

        // 发送消息到master
        void SendTaskToMaster();

    private:
        uint32 _deploy_server_id;

        // 当前执行的任务
        KFDeployTask* _kf_task;

        // 部署任务队列
        std::list< KFDeployTask* > _deploy_task;
    };
}

#endif