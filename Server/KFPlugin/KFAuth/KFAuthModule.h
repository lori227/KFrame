#ifndef __KF_AUTH_MODULE_H__
#define __KF_AUTH_MODULE_H__

/************************************************************************
//    @Module			:    登录认证模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFrame.h"
#include "KFInterfaces.h"
#include "KFAuthInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFChannel/KFChannelInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"

namespace KFrame
{
    class KFAuthModule : public KFAuthInterface
    {
    public:
        KFAuthModule();
        ~KFAuthModule();

        // 初始化
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 处理登录请求
        std::string HandleAuthChannelLogin( const std::string& ip, const std::string& data );

        // 激活请求
        std::string HandleAuthActivation( const std::string& ip, const std::string& data );

    };
}



#endif