#ifndef __KF_DIR_PROXY_MODULE_H__
#define __KF_DIR_PROXY_MODULE_H__

/************************************************************************
//    @Module			:	 目录代理转发模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-8-20
************************************************************************/

#include "KFrame.h"
#include "KFDirProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    class KFDirProxyModule : public KFDirProxyInterface
    {
    public:
        KFDirProxyModule();
        ~KFDirProxyModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 更新在线数量
        __KF_MESSAGE_FUNCTION__( HandleUpdateOnlineToDirReq );

    protected:
        // 断开连接
        __KF_SERVER_LOST_FUNCTION__( OnServerLostClient );
    };
}



#endif