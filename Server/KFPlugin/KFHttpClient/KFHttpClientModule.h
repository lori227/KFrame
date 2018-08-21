#ifndef __KF_HTTP_CLIENT_MODULE_H__
#define __KF_HTTP_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    http客户端
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-10
************************************************************************/

#include "KFrame.h"
#include "KFHttpClientInterface.h"

namespace KFrame
{
    class KFHttpClientManage;
    class KFHttpClientModule : public KFHttpClientInterface
    {
    public:
        KFHttpClientModule();
        ~KFHttpClientModule();

        // 逻辑
        virtual void BeforeRun();
        virtual void Run();

        virtual void ShutDown();
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////

        virtual std::string StartSTHttpClient( const std::string& url, const std::string& data );
        virtual std::string StartSTHttpsClient( const std::string& url, const std::string& data );

        virtual std::string StartSTHttpClient( const std::string& url, KFJson& json, bool sign );
        virtual std::string StartSTHttpsClient( const std::string& url, KFJson& json, bool sign );

        virtual void StartMTHttpClient( const std::string& url, const std::string& data );
        virtual void StartMTHttpsClient( const std::string& url, const std::string& data );

        virtual void StartMTHttpClient( const std::string& url, KFJson& json, bool sign );
        virtual void StartMTHttpsClient( const std::string& url, KFJson& json, bool sign );

        /////////////////////////////////////////////////////////////////////
        // 返回错误
        virtual std::string SendResponseCode( uint32 code );

        // 获得错误码
        virtual uint32 GetResponseCode( KFJson& json );

        // 发送json
        virtual std::string SendResponse( KFJson& json );

        // 创建签名
        virtual void MakeSignature( KFJson& json );

        // 验证签名
        virtual bool VerifySignature( KFJson& json );
    protected:
        // http
        virtual void StartMTHttpClient( const std::string& url, const std::string& data, KFHttpClientFunction& function );
        virtual void StartMTHttpClient( const std::string& url, KFJson& json, KFHttpClientFunction& function, bool sign );

        // https
        virtual void StartMTHttpsClient( const std::string& url, const std::string& data, KFHttpClientFunction& function );
        virtual void StartMTHttpsClient( const std::string& url, KFJson& json, KFHttpClientFunction& function, bool sign );

    private:
        // 客户端管理
        KFHttpClientManage* _kf_http_manage;
    };
}



#endif