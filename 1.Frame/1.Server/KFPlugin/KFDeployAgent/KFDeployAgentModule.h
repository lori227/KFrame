#ifndef __KF_DEPLOY_AGENT_MODULE_H__
#define __KF_DEPLOY_AGENT_MODULE_H__


/************************************************************************
//    @Module			:    部署Agent
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-2
************************************************************************/

#include "KFDeploy.h"
#include "KFDeployAgentInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFMySQL/KFMySQLInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFDeployTask
    {
    public:
        KFDeployTask()
        {
            _zone_id = _invalid_int;
            _start_time = _invalid_int;
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
        std::string _app_id;

        // zoneid
        uint32 _zone_id;

        // logurl
        std::string _log_url;

        // 开始时间
        uint64 _start_time;
    };


    class KFLaunchSetting;
    class KFDeployAgentModule : public KFDeployAgentInterface
    {
    public:
        KFDeployAgentModule() = default;
        ~KFDeployAgentModule() = default;

        // 加载
        virtual void AfterLoad();

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

        // 定时删除残留的版本
        __KF_SCHEDULE_FUNCTION__( OnScheduleRemoveVersion );

    protected:
        // 启动服务器
        __KF_MESSAGE_FUNCTION__( HandleDeployCommandReq );

    protected:
        // 连接deployserver
        void StartConnectDeployServer();

        // 判断是否agent进程
        bool IsAgentDeploy( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid );

        // 更新数据到部署服务
        void UpdateDeployToDatabase( KFDeployData* deploydata );

        // 启动服务器进程
        void StartupServerProcess( KFDeployData* deploydata );

        // 检查服务器进程
        void CheckServerProcess( KFDeployData* deploydata );

        // 绑定继承
        void BindServerProcess();

        // 杀死进程
        void KillServerProcess( uint32 processid );

        // 保存进程信息到文件中
        std::string FormatPidFileName( KFDeployData* deploydata );

        void SaveProcessToFile( KFDeployData* deploydata );
        void ReadProcessFromFile( KFDeployData* deploydata );

        // 获得部署路径
        void FindAppDeployPath( const std::string& appname, std::set< std::string >& deploypathlist );

#if __KF_SYSTEM__ == __KF_WIN__
        // 启动进程
        bool StartupWinProcess( KFDeployData* deploydata );

        // 检查进程是否存在
        void CheckWinProcess( KFDeployData* deploydata );

        // 杀死进程
        void KillWinProcess( uint32 processid );
#else

        template<typename... P>
        std::string ExecuteShell( const char* myfmt, P&& ... args )
        {
            auto command = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ExecuteShellCommand( command );
        }

        // 执行脚本命令
        std::string ExecuteShellCommand( const std::string& command );

        // 启动进程
        bool StartupLinuxProcess( KFDeployData* deploydata );

        // 获得linux进程id
        uint32 FindProcessIdByName( KFDeployData* deploydata );

        // 检查进程是否存在
        void CheckLinuxProcess( KFDeployData* deploydata );

        // 杀死进程
        void KillLinuxProcess( uint32 processid );

#endif
    protected:
        // 添加部署任务
        void AddDeployTask( const std::string& command, KFMsg::PBDeployCommand* pbdeploy );

        // 开始任务
        void StartDeployTask();
        void StartKillServerTask();
        void StartShutDownServerTask();
        void StartStartupServerTask();
        void StartWgetVersionTask();
        void StartDownFileTask();

        // 检查任务完成
        bool CheckTaskFinish();
        bool CheckShutDownServerTaskFinish();
        bool CheckStartupServerTaskFinish();
        bool CheckWgetVersionTaskFinish();
        bool CheckDownFileTaskFinish();

        // 发送消息到master
        void SendTaskToMaster();

        // 加载启动信息
        void LoadTotalLaunchData();

    private:
        // 回发日志消息
        template<typename... P>
        void LogDeploy( const std::string& url, const char* myfmt, P&& ... args )
        {
            auto msg = __FORMAT__( myfmt, std::forward<P>( args )... );
            SendLogMessage( url, msg );
        }

        void SendLogMessage( const std::string& url, const std::string& msg );
    private:
        // 部署服务器的
        uint64 _deploy_server_id{ 0 };

        // mysql
        KFMySQLDriver* _deploy_driver{ nullptr };

        // deploy 表名字
        std::string _deploy_table_name;

        // deploy列表
        KFMap< std::string, const std::string&, KFDeployData > _deploy_list;

        // 当前执行的任务
        KFDeployTask* _kf_task{ nullptr };

        // 部署任务队列
        std::list< KFDeployTask* > _deploy_task;
    };
}

#endif