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
#include "KFHttpPort.h"
#include "KFHttpServerInterface.h"

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
        virtual void AfterLoad();

        // 运行
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 卸载
        virtual void UnRegisterFunction( const std::string& url );

        // http完整路径
        virtual const std::string& GetHttpUrl();

        // 返回错误
        virtual std::string SendResponseCode( uint32 code );

        // 获得错误码
        virtual uint32 GetResponseCode( KFJson& json );

        // 发送json
        virtual std::string SendResponse( KFJson& json );
        virtual std::string SendResponse( KFJson& json, uint32 code );

        // 创建签名
        virtual void MakeSignature( KFJson& json );

        // 验证签名
        virtual bool VerifySignature( KFJson& json );
    private:
        void Run();

        // 注册httphandle
        virtual void RegisterMethodFunction( const std::string& url, bool sync, KFHttpMethodFunction& function );

    private:

        // 获得内网ip
        std::string GetLocalIp();
#if __KF_SYSTEM__ == __KF_WIN__
        std::string GetWinLocalIp();
#else
        std::string GetLinuxLocalIp();
#endif

    private:
        // http服务器
        KFHttpServer* _http_server;

        // 完整路径
        std::string _full_url;

        KFHttpProt _http_port;
    };
}



#endif