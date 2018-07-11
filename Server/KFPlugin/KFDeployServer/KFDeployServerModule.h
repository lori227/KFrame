#ifndef __KF_DEPLOY_SERVER_MODULE_H__
#define __KF_DEPLOY_SERVER_MODULE_H__


/************************************************************************
//    @Module			:    部署Server
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-2
************************************************************************/

#include "KFDeployDefine.h"
#include "KFDeployServerInterface.h"
#include "KFMySQL/KFMySQLInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFDeployServerModule : public KFDeployServerInterface
    {
    public:
        KFDeployServerModule();
        ~KFDeployServerModule();

        // 逻辑
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void ShutDown();

    protected:
        // 连接丢失
        __KF_SERVER_LOST_FUNCTION__( OnServerLostClient );


    protected:
        // 启动服务器
        __KF_HTTP_FUNCTION__( HandleStartupServer );

        // 关闭服务器
        __KF_HTTP_FUNCTION__( HandleShutDownServer );

        // 杀死服务器
        __KF_HTTP_FUNCTION__( HandleKillServer );

    protected:

        // 注册Agent
        __KF_MESSAGE_FUNCTION__( HandleRegisterAgentToServerReq );

        // 更新服务器状态
        __KF_MESSAGE_FUNCTION__( HandleUpdateServerStatusReq );

        // 请求分配Agent连接
        __KF_MESSAGE_FUNCTION__( HandleGetAgentIpAddressReq );

    protected:
        // 开启服务器
        void StartupServerToAgent( uint32 id, const char* data, uint32 size );

        // 关闭服务器
        void ShutDownServerToAgent( uint32 id, const char* data, uint32 size );
    private:

        // mysql
        KFMySQLDriver* _mysql_driver;

        // Agent列表
        KFMap< uint32, uint32, KFAgentData > _agent_list;
    };
}

#endif