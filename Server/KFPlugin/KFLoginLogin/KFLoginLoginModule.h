#ifndef __KF_LOGIN_LOGIN_MODULE_H__
#define __KF_LOGIN_LOGIN_MODULE_H__

/************************************************************************
//    @Moudle			:    登录客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-18
************************************************************************/

#include "KFrame.h"
#include "KFLoginLoginInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFLogin/KFLoginInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFDataClient/KFDataClientInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFDeployCommand/KFDeployCommandInterface.h"

namespace KFrame
{
    class KFLoginLoginModule : public KFLoginLoginInterface
    {
    public:
        KFLoginLoginModule();
        ~KFLoginLoginModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:

        // 登录验证请求
        __KF_MESSAGE_FUNCTION__ ( HandleLoginVerifyReq );

        // 登录验证结果
        __KF_MESSAGE_FUNCTION__ ( HandleLoginVerifyAck );

        // 查询角色playerid
        __KF_MESSAGE_FUNCTION__ ( HandleLoginQueryPlayerIdAck );

    protected:
        // 连接丢失
        __KF_SERVER_LOST_FUNCTION__( OnServerLostHandle );

        // 关闭回调
        __KF_COMMAND_FUNCTION__( OnDeployShutDownServer );

    protected:
        // 平台验证回调
        void OnHttpPlatformLoginVerifyCallBack( std::string& senddata, std::string& recvdata );

        // 发送登录验证结果消息
        void SendLoginVerifyMessage( uint32 gateid, uint32 result, uint32 accountid, uint32 playerid, const std::string& token, const std::string& ip, uint32 port );

    private:
        // 关闭登录
        bool _is_login_close;
    };
}



#endif