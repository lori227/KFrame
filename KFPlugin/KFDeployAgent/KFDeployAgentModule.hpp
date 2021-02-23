#ifndef __KF_DEPLOY_AGENT_MODULE_H__
#define __KF_DEPLOY_AGENT_MODULE_H__


/************************************************************************
//    @Module			:    部署Agent
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-2
************************************************************************/

#include "KFDeploy.hpp"
#include "KFDeployAgentInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFMySQL/KFMySQLInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

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

        // zone_id
        uint32 _zone_id;

        // 开始时间
        uint64 _start_time;
    };


    class KFLaunchSetting;
    class KFDeployAgentModule : public KFDeployAgentInterface
    {
    public:
        KFDeployAgentModule() = default;
        ~KFDeployAgentModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void ShutDown();
    protected:
        // 连接成功
        __KF_NET_EVENT_FUNCTION__( OnClientConnectServer );

        // 断开连接
        __KF_NET_EVENT_FUNCTION__( OnClientLostServer );

        // client连接连接
        __KF_NET_EVENT_FUNCTION__( OnServerDiscoverClient );

        // client断开连接
        __KF_NET_EVENT_FUNCTION__( OnServerLostClient );

        // 启动服务器
        __KF_TIMER_FUNCTION__( OnTimerStartupProcess );

        // 判断超时定时器
        __KF_TIMER_FUNCTION__( OnTimerCheckHeartbeat );

        // 检查任务完成
        __KF_TIMER_FUNCTION__( OnTimerCheckTaskFinish );

        // 查询启动信息
        __KF_TIMER_FUNCTION__( OnTimerQueryAgentData );

    protected:
        // 启动服务器
        __KF_MESSAGE_FUNCTION__( HandleDeployCommandReq, KFMsg::S2SDeployCommandToAgentReq );

        // 心跳消息
        __KF_MESSAGE_FUNCTION__( HandleClientHeartbeatReq, KFMsg::S2SDeployHeartbeatToAgentReq );

    protected:
        // 连接deployserver
        void StartConnectDeployServer();

        // 判断是否agent进程
        bool IsAgentDeploy( const std::string& command, const std::string& app_name, const std::string& app_type, const std::string& app_id, uint32 zone_id );

        // 更新数据到部署服务
        void UpdateDeployToDatabase( std::shared_ptr<KFDeployData> deploy_data );

        // 启动服务器进程
        void StartupServerProcess( std::shared_ptr<KFDeployData> deploy_data );

        // 检查服务器进程
        void CheckServerProcess( std::shared_ptr<KFDeployData> deploy_data );

        // 绑定继承
        void BindServerProcess();

        // 杀死进程
        void KillServerProcess( uint32 process_id );

        // 保存进程信息到文件中
        std::string FormatPidFileName( std::shared_ptr<KFDeployData> deploy_data );

        void SaveProcessToFile( std::shared_ptr<KFDeployData> deploy_data );
        void ReadProcessFromFile( std::shared_ptr<KFDeployData> deploy_data );

        // 获得部署路径
        void FindAppDeployPath( const std::string& app_name, uint32 zone_id, StringSet& deploy_path_list );

#if __KF_SYSTEM__ == __KF_WIN__
        // 启动进程
        bool StartupWinProcess( std::shared_ptr<KFDeployData> deploy_data );

        // 检查进程是否存在
        void CheckWinProcess( std::shared_ptr<KFDeployData> deploy_data );

        // 杀死进程
        void KillWinProcess( uint32 process_id );
#else
        template<typename... P>
        std::string ExecuteShell( const char* my_fmt, P&& ... args )
        {
            auto command = __FORMAT__( my_fmt, std::forward<P>( args )... );
            return ExecuteShellCommand( command );
        }

        // 执行脚本命令
        std::string ExecuteShellCommand( const std::string& command );

        // 启动进程
        bool StartupLinuxProcess( std::shared_ptr<KFDeployData> deploy_data );

        // 获得linux进程id
        uint32 FindProcessIdByName( std::shared_ptr<KFDeployData> deploy_data );

        // 检查进程是否存在
        void CheckLinuxProcess( std::shared_ptr<KFDeployData> deploy_data );

        // 杀死进程
        void KillLinuxProcess( uint32 process_id );

#endif
    protected:
        // 添加部署任务
        void AddDeployTask( const std::string& command, const KFMsg::PBDeployCommand* deploy_command );

        // 开始任务
        void StartDeployTask();
        void StartKillServerTask();
        void StartShutDownServerTask();
        void StartWaitTask();
        void StartStartupServerTask();
        void StartWgetVersionTask();
        void StartCopyVersionTask();
        void StartDownFileTask();
        void StartWgetResourceTask();
        void StartDownPluginTask();
        void StartDevelopUpdateTask();

        // 检查任务完成
        bool CheckTaskFinish();
        bool CheckShutDownServerTaskFinish();
        bool CheckWaitTaskFinish();
        bool CheckStartupServerTaskFinish();
        bool CheckWgetVersionTaskFinish();
        bool CheckDownFileTaskFinish();
        bool CheckDownResourceTaskFinish();
        bool CheckDownPluginTaskFinish();

        // 发送消息到master
        void SendTaskToMaster();

        // 加载启动信息
        void LoadTotalLaunchData();

        // 更新命令到数据库
        void UpdateCommandToDatabase();

    private:
        // 回发日志消息
        template<typename... P>
        void LogDeploy( const char* my_fmt, P&& ... args )
        {
            auto msg = __FORMAT__( my_fmt, std::forward<P>( args )... );
            SendLogMessage( msg );
        }
        void SendLogMessage( const std::string& msg );

    private:

        // 部署服务器
        uint64 _deploy_server_id = 0u;
        std::string _str_deploy_server_id;
        std::string _deploy_server_ip;
        uint32 _deploy_server_port = 0u;

        // mysql
        std::shared_ptr<KFMySQLDriver> _deploy_driver = nullptr;

        // deploy 表名字
        std::string _deploy_table_name;

        // deploy列表
        KFHashMap<std::string, KFDeployData> _deploy_list;

        // 当前执行的任务
        std::shared_ptr<KFDeployTask> _deploy_task = nullptr;

        // 部署任务队列
        std::list<std::shared_ptr<KFDeployTask>> _deploy_task_list;
    };
}

#endif