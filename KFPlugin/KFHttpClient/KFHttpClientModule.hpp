#ifndef __KF_HTTP_CLIENT_MODULE_H__
#define __KF_HTTP_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    http客户端
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-10
************************************************************************/

#include "KFHttpClientInterface.h"
#include "KFHttpThread.hpp"

namespace KFrame
{
    class KFHttpClientModule : public KFHttpClientInterface
    {
    public:
        KFHttpClientModule() = default;
        ~KFHttpClientModule();

        // 逻辑
        virtual void BeforeRun();
        virtual void Run();

        virtual void BeforeShut();
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        // 单线程
        virtual std::string STGet( const std::string& url, KFJson& json );
        virtual std::string STGet( const std::string& url, const std::string& data );

        virtual std::string STPost( const std::string& url, KFJson& json );
        virtual std::string STPost( const std::string& url, const std::string& data );
        /////////////////////////////////////////////////////////////////////
        // 返回错误
        virtual std::string SendCode( uint32 code );

        // 获得错误码
        virtual uint32 GetCode( KFJson& json );

        // 发送json
        virtual std::string SendResponse( KFJson& json );

    protected:
        virtual void MTGetRequest( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& args );
        virtual void MTPostRequest( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& args );

        virtual void MTGetRequest( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& args );
        virtual void MTPostRequest( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& args );

    private:
        // 添加异步请求
        void AddHttpData( KFHttpData* httpdata );

        // 判断是否是https
        bool IsHttpsClient( const std::string& url );

        // 创建http线程
        void CreateHttpThread();
        /////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
        std::string STRequest( uint32 type, const std::string& url, const std::string& data );
        void MTRequest( uint32 type, KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& args );

    private:
        // http线程列表
        std::vector< KFHttpThread* > _http_thread_list;

        // 当前的索引
        uint32 _thread_index = 0u;
    };
}



#endif