#ifndef __KF_HTTP_CLIENT_MANAGE_H__
#define __KF_HTTP_CLIENT_MANAGE_H__

/************************************************************************



************************************************************************/
#include "KFrame.h"

namespace KFrame
{
    class KFHttpData;
    class KFHttpClientManage
    {
    public:
        KFHttpClientManage();
        ~KFHttpClientManage();

        // 初始化
        void Initialize();

        // 关闭
        void ShutDown();

        // 执行逻辑
        void RunUpdate();

        // 同步http请求
        std::string SendSTHttp( const std::string& url, const std::string& data );
        std::string SendSTHttps( const std::string& url, const std::string& data );

        // 异步http请求
        void SendMTHttp( const std::string& url, const std::string& data, KFHttpClientFunction& function );
        void SendMTHttps( const std::string& url, const std::string& data, KFHttpClientFunction& function );

    protected:
        // 添加异步请求
        void AddHttpData( KFHttpData* httpdata );

        // http请求
        void RunHttpRequest();

    private:
        volatile bool _thread_run;

        // 请求的数据队列
        KFMutex _kf_req_mutex;
        std::list< KFHttpData* > _req_http_data;

        // 完成的数据队列
        KFMutex _kf_ack_mutex;
        std::list< KFHttpData* > _ack_http_data;
    };

}
#endif