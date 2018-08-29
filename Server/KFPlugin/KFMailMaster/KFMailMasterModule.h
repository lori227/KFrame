#ifndef __KF_MAIL_MASTER_MODULE_H__
#define __KF_MAIL_MASTER_MODULE_H__

/************************************************************************
//    @Module			:    目录服务逻辑
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFrame.h"
#include "KFMailMasterInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    class KFMailMasterModule : public KFMailMasterInterface
    {
    public:
        KFMailMasterModule() = default;
        ~KFMailMasterModule() = default;

        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut ();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 请求http列表
        __KF_HTTP_FUNCTION__( HandleAddMail );

    protected:
        // 发现连接
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDiscoverClient );

        // 连接断开
        __KF_SERVER_LOST_FUNCTION__( OnClientLostServer );
    private:
        // hash一致性列表
        KFConHash _kf_hash;
    };
}

#endif