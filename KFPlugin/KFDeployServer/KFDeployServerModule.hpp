#ifndef __KF_DEPLOY_SERVER_MODULE_H__
#define __KF_DEPLOY_SERVER_MODULE_H__


/************************************************************************
//    @Module			:    部署Server
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-2
************************************************************************/

#include "KFDeployServerInterface.h"
#include "KFMySQL/KFMySQLInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDelayed/KFDelayedInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
    class KFAgentData
    {
    public:
        KFAgentData()
        {
            _port = 0;
            status = 0;
        }

    public:
        // 服务器id
        std::string _agent_id;

        // 局域网地址
        std::string _local_ip;

        // 名字
        std::string _name;

        // 类型
        std::string _type;

        // 服务
        std::string _service;

        // 端口
        uint32 _port;

        // 状态
        uint32 status;
    };


    class KFDeployServerModule : public KFDeployServerInterface
    {
    public:
        KFDeployServerModule() = default;
        ~KFDeployServerModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void ShutDown();

    protected:
        // 连接丢失
        __KF_NET_EVENT_FUNCTION__( OnServerLostClient );

        // 启动服务器
        __KF_HTTP_FUNCTION__( HandleDeployCommand );

        // 部署命令
        __KF_DELAYED_FUNCTION__( OnHttpDeployCommandToAgent );
        __KF_DELAYED_FUNCTION__( OnTcpDeployCommandToAgent );

    protected:
        // 注册Agent
        __KF_MESSAGE_FUNCTION__( HandleRegisterAgentToServerReq, KFMsg::S2SRegisterAgentToServerReq );

        // 执行sql语句
        __KF_MESSAGE_FUNCTION__( HandleExecuteMySQLReq, KFMsg::S2SDeployToolExecuteMySQLReq );

        // 查询sql语句
        __KF_MESSAGE_FUNCTION__( HandleQueryMySQLReq, KFMsg::S2SDeployToolQueryMySQLReq );

        // 查询sql语句
        __KF_MESSAGE_FUNCTION__( HandleDeleteMySQLReq, KFMsg::S2SDeployToolDeleteMySQLReq );

        // 部署命令
        __KF_MESSAGE_FUNCTION__( HandleDeployToolCommandReq, KFMsg::S2SDeployToolCommandReq );

        // 日志
        __KF_MESSAGE_FUNCTION__( HandleDeployLogToServerAck, KFMsg::S2SDeployLogToServerAck );

        // 查询toolid
        __KF_MESSAGE_FUNCTION__( HandleDeployQueryToolIdReq, KFMsg::S2SDeployToolQueryToolIdReq );

    protected:
        // 更新Agnet状态
        void UpdateAgentToDatabase( std::shared_ptr<KFAgentData> agent_data, uint32 status );

        // 回发日志消息
        template<typename... P>
        void LogDeploy( uint64 agent_id, const char* fmt, P&& ... args )
        {
            auto msg = __FORMAT__( fmt, std::forward<P>( args )... );
            return SendLogMessage( agent_id, msg );
        }

        void SendLogMessage( uint64 agent_id, const std::string& msg );

    private:
        std::shared_ptr<KFMySQLDriver> _mysql_driver = nullptr;

        // Agent列表
        KFHashMap<std::string, KFAgentData> _agent_list;

        // 定时任务id
        uint64 _delayed_id = 0u;

        // web列表
        std::string _web_deploy_url;
    };
}

#endif