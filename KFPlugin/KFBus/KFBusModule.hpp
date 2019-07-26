#ifndef __KF_BUS_MODULE_H__
#define __KF_BUS_MODULE_H__

/************************************************************************
//    @Module			:    Bus总线连接模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-4
************************************************************************/

#include "KFBusInterface.h"
#include "KFBusConfig.hpp"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    class KFBusModule : public KFBusInterface
    {
    public:
        KFBusModule() = default;
        ~KFBusModule() = default;

        // 初始化
        virtual void InitModule();

        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 通知注册
        __KF_MESSAGE_FUNCTION__( HanldeTellRegisterToServer );

        // 取消注册
        __KF_MESSAGE_FUNCTION__( HanldeTellUnRegisterFromServer );

    protected:
        // 查找连接列表
        void FindConnection( IpAddressList& outlist );

        // 判断是否需要连接
        bool IsConnection( const std::string& connectname, const std::string& connecttype, uint64 connectid );
    };
}

#endif