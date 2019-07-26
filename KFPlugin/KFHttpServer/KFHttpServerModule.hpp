#ifndef __KF_HTTP_SERVER_MODULE_H__
#define __KF_HTTP_SERVER_MODULE_H__

/************************************************************************
//    @Module			:    http服务器
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-10
************************************************************************/

#include "KFrame.h"
#include "KFHttpServerInterface.h"
#include "KFHttpServerConfig.hpp"
#include "KFConfig/KFConfigInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"

namespace KFrame
{
    class KFHttpServer;
    class KFHttpServerModule : public KFHttpServerInterface
    {
    public:
        KFHttpServerModule();
        ~KFHttpServerModule();

        // 初始化
        virtual void InitModule();

        // 运行
        virtual void BeforeRun();
        virtual void Run();

        // 关闭
        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 卸载
        virtual void UnRegisterFunction( const std::string& url );

        // http完整路径
        virtual const std::string& GetHttpUrl();

        // 返回错误
        virtual std::string SendCode( uint32 code );

        // 获得错误码
        virtual uint32 GetCode( KFJson& json );

        // 发送json
        virtual std::string SendResponse( KFJson& json );
        virtual std::string SendResponse( KFJson& json, uint32 code );
    private:

        // 注册httphandle
        virtual void RegisterMethodFunction( const std::string& url, bool sync, KFHttpMethodFunction& function );
    private:
        // http服务器
        KFHttpServer* _http_server;

        // 完整路径
        std::string _full_url;
    };
}



#endif