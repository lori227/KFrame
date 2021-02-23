#ifndef __KF_HTTP_THREAD_H__
#define __KF_HTTP_THREAD_H__

#include "KFHttp/KFHttpData.h"

namespace KFrame
{
    class KFHttpThread
    {
    public:
        KFHttpThread();
        virtual ~KFHttpThread();

        // 关闭
        void ShutDown();

        // 添加异步请求
        void AddHttpRequest( std::shared_ptr<KFHttpData> http_data );

        // http相应
        void RunHttpResponse();
    protected:
        // http请求
        void RunHttpRequest();
    private:
        std::atomic<bool> _thread_run;

        // 请求的数据队列
        KFMutex _kf_req_mutex;
        std::list<std::shared_ptr<KFHttpData>> _http_request_list;

        // 完成的数据队列
        KFQueue<KFHttpData> _http_response_list;
    };
}



#endif