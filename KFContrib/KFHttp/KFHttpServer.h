#ifndef __KF_HTTP_SERVER_H__
#define __KF_HTTP_SERVER_H__

#include "KFrame.h"

namespace Poco
{
    namespace Net
    {
        class HTTPServer;
    }
}

namespace KFrame
{
    class KFHttpFunction
    {
    public:
        // http函数
        KFHttpMethodFunction _function;

        // 是否同步( 在主逻辑线程执行 )
        bool _sync = true;
    };

    class KFFunctionData
    {
    public:
        // 执行函数
        std::shared_ptr<KFHttpFunction> _http_function = nullptr;

        // ip
        std::string _ip;

        // 数据
        std::string _data;
    };

    class KFHttpServer
    {
    public:

        KFHttpServer() = default;
        ~KFHttpServer();

        ////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////
        // 开启
        void Start( const std::string& ip, uint32 port, uint32 max_thread, uint32 max_queue, uint32 idle_time, bool keep_live );

        // 逻辑
        void Run();

        // 关闭
        void ShutDown();
        ///////////////////////////////////////////////////////////////////
        // 注册创建httphandle
        void RegisterMethodFunction( const std::string& url, bool sync, KFHttpMethodFunction& function );

        // 卸载
        void UnRegisterMethodFunction( const std::string& url );

        // 处理http请求
        std::string ProcessHttpRequest( const std::string& url, const std::string& ip, const std::string& data );

    private:
        Poco::Net::HTTPServer* _http_server = nullptr;

        // 函数列表
        KFHashMap< std::string, KFHttpFunction > _functions;

        // 线程锁
        KFMutex _kf_mutex;

        // 请求数据
        std::list< std::shared_ptr<KFFunctionData> > _function_data_list;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

}


#endif