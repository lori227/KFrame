#ifndef __KF_CLUSTER_SERVER_MODULE_H__
#define __KF_CLUSTER_SERVER_MODULE_H__

/************************************************************************
//    @Moudle			:    集群服务器模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-17
************************************************************************/

#include "KFrame.h"
#include "KFClusterServerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFClusterServerModule : public KFClusterServerInterface
    {
    public:
        KFClusterServerModule();
        ~KFClusterServerModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 注册
        __KF_MESSAGE_FUNCTION__( HandleClusterRegisterReq );

        // 更新连接数量
        __KF_MESSAGE_FUNCTION__( HandleClusterUpdateReq );

        // 登录认证
        __KF_MESSAGE_FUNCTION__( HandleClusterAuthReq );

    protected:
        // 连接丢失
        __KF_SERVER_LOST_FUNCTION__( OnServerLostClusterProxy );

    private:
        // 生成认证token
        std::string MakeAuthToken( const KFGuid& guid );

    private:
        // 集群认证秘钥
        std::string _cluster_key;

        // 序列号
        uint32 _cluster_serial;
    };
}



#endif